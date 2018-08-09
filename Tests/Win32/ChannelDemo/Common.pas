unit Common;

interface

uses
  Classes;

const
  ProgName = 'ChannelDemo';

  ComPortRatesCount = 13;
  ComPortRates : array[0 .. ComPortRatesCount-1] of Integer = (
                    110,
                    300,
                    600,
                   1200,
                   2400,
                   4800,
                   9600,
                  14400,
                  19200,
                  38400,
                  56000,
                  57600,
                  115200);

  ComPortParityCount = 5;
  ComPortParity : array[0..ComPortParityCount-1] of String =
                                  ('None','Odd','Even','Mark','Space');


  IniSectPort           = 'Port';
  IniKeyPortNum         = 'Num';
  IniKeyPortRate        = 'Rate';
  IniKeyPortParity      = 'Parity';

  IniSectDCON           = 'DCON';
  IniKeyCheckSum        = 'CheckSum';

  IniSectModule         = 'Module';
  IniKeyModuleName      = 'Name';
  IniKeyModuleAdress    = 'Adress';

  IniSectDat            = 'Dat';
  IniKeyDatType         = 'DatType';
  IniKeyPressure        = 'Pressure';
  IniKeyLastValue       = 'LastValue';

  IniSectConfig         = 'Config';
  IniKeyLastLoadConfig  = 'LastLoadConfig';

  ComBufSize = 128;

  ComTypeGet     = 0;
  ComTypeSend    = 1;
  ComTypeGetDust = 2;

type
  TDconCommand = record
    tip : integer;
    LeadingChar : char;
    Address : integer;
    Command : string;
    Time : cardinal;
    ID : Cardinal;
    CS : boolean;
  end;

  TComPortUst = record
    Port: String;
    Rate: integer;
    Parity: integer;
  end;

var
  ProgPath : string;
  IniFileName : string;
  IdemAddress : integer;
  IdemAddressStr : string;
  hComPort : Cardinal;
  CommandID : Cardinal;
  ComBuf : array[0..ComBufSize-1] of TDconCommand;
  ComBufInd : integer;
  LastDrawComID : Cardinal; //Идентификатор последней нарисованной команды
  LastDrawPos : integer;

  ModuleNameStr : string;
  ModuleConfStr : string;

  AI_Value1 : double;

  DatType : integer;
  DatTypeMin : array[0..1] of integer;
  DatTypeMax : array[0..1] of integer;
  bPressure : boolean;

  bUseCheckSum : boolean;

function DConFloatValueString(Value : double) : string;

function DConCalcCS(cStr : string) : string;

function DConCheckCS(cStr : string) : boolean;

function yStrToFloatDef(inStr: string; DefValue: Double; DecSep: char = '.') : double;

procedure EnumSerialPorts(items: TStrings);

implementation

uses
  SysUtils, Windows;

procedure EnumSerialPorts(items: TStrings);
const
  BufSize     = 65535;
var
  Buf_DevList : Array[0..BufSize] of Char;
  DevName     : PChar;
  sDevName    : string;
  bSuccess    : boolean;
  i           : integer;
  sPort       : string;
  hPort       : THandle;
  dwError     : Cardinal;
begin
  //Make sure we clear out any elements which may already be in the array
  items.clear;

  //On NT use the QueryDosDevice API
  if (Win32Platform = VER_PLATFORM_WIN32_NT) then
  begin
    //Use QueryDosDevice to look for all devices of the form COMx. This is a better
    //solution as it means that no ports have to be opened at all.
    if QueryDosDevice(nil, Buf_DevList, BufSize) = 0 then Exit;
    DevName := Buf_DevList;
    while DevName^ <> #00 do
    begin
      if (StrLIComp('COM', DevName, 3) = 0) then
      begin
        sDevName := StrPas(DevName);
        items.Add(sDevName);
      end;
      DevName := StrEnd(DevName)+1;
    end;
  end else
  begin
    //On 95/98 open up each port to determine their existence

    //Up to 255 COM ports are supported so we iterate through all of them seeing
    //if we can open them or if we fail to open them, get an access denied or general error error.
    //Both of these cases indicate that there is a COM port at that number.
    for i := 1 to 16 do
    begin
      //Form the Raw device name
      sPort := Format('\\.\COM%d', [i]);

      //Try to open the port
      bSuccess := FALSE;
      hPort := CreateFile(PChar(sPort), GENERIC_READ or GENERIC_WRITE, 0, 0,
OPEN_EXISTING, 0, 0);
      if (hPort = INVALID_HANDLE_VALUE) then
      begin
        dwError := GetLastError();

        //Check to see if the error was because some other app had the port open or a general failure
        if (dwError = ERROR_ACCESS_DENIED) or (dwError = ERROR_GEN_FAILURE)
then bSuccess := TRUE;
      end else
      begin
        //The port was opened successfully
        bSuccess := TRUE;

        //Don't forget to close the port, since we are going to do nothing withit anyway
        CloseHandle(hPort);
      end;

      //Add the port number to the array which will be returned
      if (bSuccess) then items.Add(Format('COM%d', [i]));
    end;
  end;
end;

function DConFloatValueString(Value : double) : string;
var
  Str : string;
  Len,i : integer;
begin
  Str := FloatToStrF(Value, ffFixed, 7, 3);
  Len := Length(Str);
  if (Value>=0) then //Положительное число - нужно добавить '+'
    begin
      if (Len<=5) then Str := '+0'+Str
      else Str := '+'+Str;
    end
  else //Отрицательное число
    if (Len<=6) then
      begin
        SetLength(Str,Len+1);
        for i:=0 to Len-1 do Str[Len-i+1]:=Str[Len-i];
        Str[2]:='0';
      end;
  result := Str;
end;

function DConCalcCS(cStr : string) : string;
var
  Len : integer;
  iCS,i : integer;
begin
  Len := Length(cStr);
  iCS := 0;
  for i:=1 to Len do
    iCS := iCS + BYTE(cStr[i]);
  iCS := iCS and $FF;
  Result := IntToHex(iCS,2);
end;

function DConCheckCS(cStr : string) : boolean;
var
  i,iCS,sCS : integer;
  Len,sLen : integer;
  CS : string;
begin
  Len := Length(cStr);
  if (Len<3) then//У строки нет контрольной суммы
    begin
      Result := FALSE;
      exit;
    end;
  sLen := Len-2;
  CS := Copy(cStr,sLen+1,2);
  iCS := 0;
  for i:=1 to sLen do
    iCS := iCS + BYTE(cStr[i]);
  iCS := iCS and $FF;
  sCS := StrToIntDef('$'+CS,-1);
  Result := (iCS = sCS);
end;

function yStrToFloatDef(inStr: string; DefValue: Double; DecSep: char) : double;
var
  Val,Mul,D : double;
  Len,i : integer;
  c : char;
  bError : boolean;
begin
  Result := 0;
  bError := FALSE;
  inStr := Trim(inStr);
  Len := Length(inStr);
  if (Len=0) then bError:=TRUE;
  Val := 0;
  Mul := 1;
  D := 0;
  for i:=0 to Len-1 do
  begin
      c := inStr[Len-i];
      case c of
        '0'..'9' :
           begin
             Val := Val + (Ord(c)-$30)*Mul;
             Mul := Mul*10;
           end;
      else
        if c=DecSep then
          begin
            if (D=0) then D := Mul
            else
              begin
                bError:=TRUE;
                break;
              end;
          end
        else
          begin
            bError:=TRUE;
            break;
          end;
      end;
  end;
  if (D>0) then
    Val := Val/D;
  if bError then
//    raise Exception.Create('Error of converting string to float Str="'+inStr+'"')
    Result := DefValue
  else
    Result := Val;
end;

end.

unit Channel;

interface

uses
  Contnrs, Classes,
  Common, SimpleXML, Automobile;

const
  CHANNEL_NUMBER_OF_STATUS = 10;

  STATUS_UNKNOWN        = 0;
  STATUS_AVAILABLE      = 1;
  STATUS_PREPARING      = 2;
  STATUS_CHARGING       = 3;
  STATUS_SUSPENDED_EVSE = 4;
  STATUS_SUSPENDED_EV   = 5;
  STATUS_FINISHING      = 6;
  STATUS_RESERVED       = 7;
  STATUS_UNAVAILABLE    = 8;
  STATUS_FAULTED        = 9;

  STATUS_STR_UNKNOWN        = 'Unknown';
  STATUS_STR_AVAILABLE      = 'Available';
  STATUS_STR_PREPARING      = 'Preparing';
  STATUS_STR_CHARGING       = 'Charging';
  STATUS_STR_SUSPENDED_EVSE = 'SuspendedEVSE';
  STATUS_STR_SUSPENDED_EV   = 'SuspendedEV';
  STATUS_STR_FINISHING      = 'Finishing';
  STATUS_STR_RESERVED       = 'Reserved';
  STATUS_STR_UNAVAILABLE    = 'Unavailable';
  STATUS_STR_FAULTED        = 'Faulted';

  CHARGE_POWER_NONE  = 0;
  CHARGE_POWER_7KWT  = 1;
  CHARGE_POWER_22KWT = 2;

type
  TOnEventStatusChanged = procedure (newStatus: Integer) of object;
  TOnEventPowerConsumed = procedure(value: Integer) of object;
  TOnEventMeterValueChanged = procedure(value: Integer) of object;

  TChargeThread = class(TThread)
  private
    FActive: Boolean;
    FOnPowerConsumed: TOnEventPowerConsumed;
  protected
    procedure Execute; override;
  public
    procedure Stop;
    property OnPowerConsumed: TOnEventPowerConsumed read FOnPowerConsumed write FOnPowerConsumed;
  end;

  TChannel = class
  private
    fAddress: Integer;
    fAdrStr: String;
    fStateOn: boolean;
    fStatus: Integer;
    fMeterOn: Boolean;
    fMeterValue: Integer;
    fOnEventStatusChanged: TOnEventStatusChanged;
    fOnEventMeterValueChanged: TOnEventMeterValueChanged;
    fChargeThread: TChargeThread;
    FAutomobile: TAutomobile;
    FChargePower: Integer;
    FPostAction: Integer;
    procedure OnPowerConsumed(value: Integer);
    procedure setStatus(const Value: Integer);
    procedure startCharging;
    procedure stopChargingThread;
    function GetAutomobileConnected: Boolean;
  public
    public Constructor Create;
    property Address: Integer read fAddress;
    property StateOn: Boolean read fStateOn;
    property Status: Integer read fStatus write setStatus;
    property MeterOn: Boolean read fMeterOn;
    property MeterValue: Integer read fMeterValue;
    function Init(node: IXmlNode): Boolean;
    procedure Load(node: IXmlNode);
    procedure Save(node: IXmlNode);
    procedure setState(state: Boolean);
    procedure setMeterOn(isOn: Boolean);
    procedure setMeterValue(value: Integer);
    function ProcessCommand(Command : TDConCommand; var OutStr : string) : boolean;
    procedure ConnectAutomobile(Automobile: TAutomobile);
    procedure UnconnectAutomobile;
    procedure haltCharging;
    procedure doPostAction;
    property IsAutomobileConnected: Boolean read GetAutomobileConnected;
    property OnEventStatusChanged: TOnEventStatusChanged read FOnEventStatusChanged write FOnEventStatusChanged;
    property OnEventMeterValueChanged: TOnEventMeterValueChanged read fOnEventMeterValueChanged write fOnEventMeterValueChanged;
  end;

  TChannelList = class
  private
    fList: TObjectList;
    function getCount: Integer;
  public
    constructor Create;
    destructor Destroy; override;
    property Count: Integer read getCount;
    function Add(channel: TChannel): Integer;
    function GetChannelByPos(index: integer): TChannel;
    function GetChannelByAddress(address: integer): TChannel;
  end;

function ChannelGetStatusString(status: Integer): String;

implementation

uses
  SysUtils, Windows;

const
  MODULE_NAME = 'ChargePointConnector';

  ATTRIB_ADDRESS = 'address';
  ATTRIB_STATE_ON    = 'stateOn';
  ATTRIB_STATUS      = 'status';
  ATTRIB_METER_ON    = 'meterOn';
  ATTRIB_METER_VALUE = 'meterValue';

  POST_ACTION_NONE           = 0;
  POST_ACTION_START_CHARGING = 1;
  POST_ACTION_HALT_CHARGING  = 2;

function ChannelGetStatusString(status: Integer): String;
begin
  Result := STATUS_STR_UNKNOWN;

  case status of
    STATUS_AVAILABLE:      Result := STATUS_STR_AVAILABLE;
    STATUS_PREPARING:      Result := STATUS_STR_PREPARING;
    STATUS_CHARGING:       Result := STATUS_STR_CHARGING;
    STATUS_SUSPENDED_EVSE: Result := STATUS_STR_SUSPENDED_EVSE;
    STATUS_SUSPENDED_EV:   Result := STATUS_STR_SUSPENDED_EV;
    STATUS_FINISHING:      Result := STATUS_STR_FINISHING;
    STATUS_RESERVED:       Result := STATUS_STR_RESERVED;
    STATUS_UNAVAILABLE:    Result := STATUS_STR_UNAVAILABLE;
    STATUS_FAULTED:        Result := STATUS_STR_FAULTED;
  end;
end;

{ TChannelList }

function TChannelList.Add(channel: TChannel): Integer;
begin
  fList.Add(channel);
  Result := fList.Count;
end;

constructor TChannelList.Create;
begin
  fList := TObjectList.Create;
end;

destructor TChannelList.Destroy;
begin
  fList.Free;
  inherited;
end;

function TChannelList.GetChannelByAddress(address: integer): TChannel;
var
  I: Integer;
  channel: TChannel;
begin
  for I := 0 to fList.Count - 1 do
  begin
    channel := fList.Items[I] as TChannel;
    if(channel.Address = address) then
    begin
      Result := channel;
      Exit;
    end;
  end;
  Result := nil;
end;

function TChannelList.GetChannelByPos(index: integer): TChannel;
begin
  if((index >= 0) and (index < fList.Count)) then
    Result := fList.Items[Index] as TChannel
  else
    Result := nil;
end;

function TChannelList.getCount: Integer;
begin
  Result := fList.Count;
end;

{ TChannel }

procedure TChannel.ConnectAutomobile(Automobile: TAutomobile);
begin
  FAutomobile := Automobile;
end;

constructor TChannel.Create;
begin
  FAutomobile := nil;
  FChargePower := CHARGE_POWER_NONE;
end;

procedure TChannel.doPostAction;
begin
  case FPostAction of
    POST_ACTION_START_CHARGING: StartCharging;
    POST_ACTION_HALT_CHARGING: HaltCharging;
  end;
end;

function TChannel.GetAutomobileConnected: Boolean;
begin
  Result := (FAutomobile <> nil);
end;

procedure TChannel.haltCharging;
begin
  FChargePower := CHARGE_POWER_NONE;
  stopChargingThread;
  if FAutomobile <> nil then
    FAutomobile.ChargingAllow := False;
end;

function TChannel.Init(node: IXmlNode): Boolean;
var
  s: String;
  iValue: Integer;
begin
  Result := False;

  fAddress := node.GetIntAttr(ATTRIB_ADDRESS, 0);
  fAdrStr := IntToHex(fAddress, 2);
  fChargeThread := nil;

  Result := True;
end;

procedure TChannel.Load(node: IXmlNode);
begin
  fStateOn := node.GetBoolAttr(ATTRIB_STATE_ON);
  fStatus := node.GetIntAttr(ATTRIB_STATUS);
  fMeterOn := node.GetBoolAttr(ATTRIB_METER_ON);
  fMeterValue := node.GetIntAttr(ATTRIB_METER_VALUE);
end;

procedure TChannel.OnPowerConsumed(value: Integer);
var
  Consumed: Integer;
begin
  Consumed := FAutomobile.ConsumeEnergy(value);
  SetMeterValue(FMeterValue + Consumed);
end;

function TChannel.ProcessCommand(Command: TDConCommand;
  var OutStr: string): boolean;
const
  HEAD_LEN = 3;
  CS_LEN = 2;
var
  Str, Answer: string;
  ServLen: integer;
begin
  Result := FALSE;
  if not fStateOn then
    Exit; //Модуль выключен

  FPostAction := POST_ACTION_NONE;

  OutStr := '';
  if (bUseCheckSum) then
    ServLen := HEAD_LEN + CS_LEN
  else
    ServLen := HEAD_LEN;
  Str := Copy(Command.Command,HEAD_LEN+1,Length(Command.Command)-ServLen);

  case (Command.LeadingChar) of
    '$' :
      begin
        if (Str='N') then
          OutStr := '!' + fAdrStr + MODULE_NAME
        else if (Str = 'S') then
          OutStr := '!' + fAdrStr + IntToStr(fStatus)
        else if (Str = 'M') then
        begin
          if fMeterOn then
            OutStr := '!' + fAdrStr + IntToStr(fMeterValue)
          else
            OutStr := '?' + fAdrStr + '0';
        end
        else if (Str = 'P') then
        begin
          if FAutomobile = nil then
            Answer := 'A'
          else
          begin
            Case FAutomobile.ResistorState of
              RESISTOR_STATE_B: Answer := 'B';
              RESISTOR_STATE_C: Answer := 'C';
              RESISTOR_STATE_D: Answer := 'D';
            else
              Answer := 'E';
            end;
          end;
          Answer := Answer + IntToStr(FChargePower);
          OutStr := '!' + fAdrStr + Answer;
        end;
          //OutStr := '>' + '22002500260027003831393140314131';
      end;
    '~' :
      begin
        OutStr := '!' + fAdrStr;
      end;
    '#' :
    begin
      if(Str = 'S') then
      begin
        if IsAutomobileConnected then
        begin
          FPostAction := POST_ACTION_START_CHARGING;
          OutStr := '!' + fAdrStr;
        end
        else
          OutStr := '?' + fAdrStr + '0';
      end
      else if(Str = 'H') then
      begin
        FPostAction := POST_ACTION_HALT_CHARGING;
        OutStr := '!' + fAdrStr;
      end
      else
        OutStr := '?' + fAdrStr;
    end;
  end;
  Result := (Length(OutStr)>0);

end;

procedure TChannel.Save(node: IXmlNode);
begin
  node.SetBoolAttr(ATTRIB_STATE_ON, fStateOn);
  node.SetIntAttr(ATTRIB_STATUS, fStatus);
  node.SetBoolAttr(ATTRIB_METER_ON, fMeterOn);
  node.SetIntAttr(ATTRIB_METER_VALUE, fMeterValue);
end;

procedure TChannel.setMeterOn(isOn: Boolean);
begin
  fMeterOn := isOn;
end;

procedure TChannel.setMeterValue(value: Integer);
begin
  if fMeterValue = value then
    Exit;

  fMeterValue := value;
  if(Assigned(fOnEventMeterValueChanged)) then
    fOnEventMeterValueChanged(fMeterValue);
end;

procedure TChannel.setState(state: Boolean);
begin
  fStateOn :=  state;
end;

procedure TChannel.setStatus(const Value: Integer);
begin
  if fStatus = Value then
    Exit;

  if(fStatus = STATUS_CHARGING) then
    StopChargingThread;

  fStatus := Value;
  if(Assigned(fOnEventStatusChanged)) then
    fOnEventStatusChanged(fStatus);
end;

procedure TChannel.startCharging;
begin
  FAutomobile.ChargingAllow := True;
  FChargePower := CHARGE_POWER_7KWT;
  if FChargeThread = nil then
  begin
    fChargeThread := TChargeThread.Create(True);
    fChargeThread.OnPowerConsumed := OnPowerConsumed;
    fChargeThread.Resume;
  end;
end;

procedure TChannel.stopChargingThread;
begin
  if fChargeThread <> nil then
  begin
    fChargeThread.Stop;
    WaitForSingleObject(fChargeThread.Handle, 1000);
    FreeAndNil(fChargeThread);
  end;
end;

procedure TChannel.UnconnectAutomobile;
begin
  FAutomobile := nil;
  FChargePower := CHARGE_POWER_NONE;
end;

{ TChargeThread }

procedure TChargeThread.Execute;
var
  LastTick, CurTick: Cardinal;
begin
  inherited;
  fActive := True;
  LastTick := GetTickCount;
  while fActive do
  begin
    Sleep(1);
    CurTick := GetTickCount;
    if((CurTick - LastTick) >= 500) then
    begin
      if Assigned(FOnPowerConsumed) then
        FOnPowerConsumed(1);
      LastTick := CurTick;  
    end;
  end;
end;

procedure TChargeThread.Stop;
begin
  fActive := False;
end;

end.

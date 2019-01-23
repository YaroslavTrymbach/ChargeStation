unit IdemFrame;

interface

uses 
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls, RXSwitch, Buttons, ExtCtrls,
  Channel;

const
  FixHeight = 68;
  DatBoxMinHeight = 30;
  DatHeight = 20;

  DatBoxNum = 2;

  DB_INDEX_DIN = 0;
  DB_INDEX_AIN = 1;

type
  TIdemFram = class(TFrame)
    gbMain: TGroupBox;
    swTurn: TRxSwitch;
    lTurnState: TLabel;
    btSetup: TSpeedButton;
    Panel1: TPanel;
    GroupBox1: TGroupBox;
    lbStatus: TLabel;
    btChangeStatus: TButton;
    Label1: TLabel;
    Label2: TLabel;
    lbError: TLabel;
    Button1: TButton;
    Label3: TLabel;
    cbCounterGood: TCheckBox;
    edCounter: TEdit;
    procedure swTurnClick(Sender: TObject);
    procedure btChangeStatusClick(Sender: TObject);
    procedure cbCounterGoodClick(Sender: TObject);
  private
    { Private declarations }
    fChannel: TChannel;
    procedure RecalcHeight;
    procedure UpdateControls;
    procedure OnStatusChanged(newState: Integer);
    procedure OnMeterValueChanged(value: Integer);
  public
    procedure SetupChannel(channel: TChannel);
    procedure SetCaption(Cap : string);
  end;

implementation

uses
  Common, ChangeStatus;

{$R *.dfm}

procedure TIdemFram.SetCaption(Cap: string);
begin
  gbMain.Caption := Cap;
end;

procedure TIdemFram.swTurnClick(Sender: TObject);
begin
  fChannel.setState(swTurn.StateOn);
end;

procedure TIdemFram.RecalcHeight;
var
  h,i : integer;
begin
  //Пересчет высоты фрейма
  h := FixHeight;
  Height := h;
end;


procedure TIdemFram.SetupChannel(channel: TChannel);
begin
  fChannel := channel;

  gbMain.Caption := 'Канал: адрес ' + IntToHex(fChannel.Address, 2);

  swTurn.StateOn := fChannel.StateOn;

  fChannel.OnEventStatusChanged := OnStatusChanged;
  fChannel.OnEventMeterValueChanged := OnMeterValueChanged;

  UpdateControls;

//  RecalcHeight;
end;

procedure TIdemFram.btChangeStatusClick(Sender: TObject);
var
  fmChangeStatus: TfmChangeStatus;
begin
  fmChangeStatus := TfmChangeStatus.Create(nil);
  fmChangeStatus.Status := fChannel.Status;
  if(fmChangeStatus.ShowModal = mrOK) then
  begin
    fChannel.Status := fmChangeStatus.Status;
    UpdateControls;
  end;
  fmChangeStatus.Free;
end;

procedure TIdemFram.UpdateControls;
begin
  if (swTurn.StateOn) then
  begin
    lTurnState.Caption := 'Включено';
    lTurnState.Font.Color := clGreen;
  end
  else
  begin
    lTurnState.Caption := 'Выключено';
    lTurnState.Font.Color := clRed;
  end;

  lbStatus.Caption := ChannelGetStatusString(fChannel.Status);

  cbCounterGood.Checked := fChannel.MeterOn;
  edCounter.Text := IntToStr(fChannel.MeterValue);
end;

procedure TIdemFram.cbCounterGoodClick(Sender: TObject);
begin
  fChannel.setMeterOn(cbCounterGood.Checked);
end;

procedure TIdemFram.OnStatusChanged(newState: Integer);
begin
  UpdateControls;
end;

procedure TIdemFram.OnMeterValueChanged(value: Integer);
begin
  UpdateControls;
end;

end.

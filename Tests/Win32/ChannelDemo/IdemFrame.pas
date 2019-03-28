unit IdemFrame;

interface

uses 
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls, RXSwitch, Buttons, ExtCtrls,
  Channel, Automobile, ComCtrls;

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
    Panel1: TPanel;
    GroupBox1: TGroupBox;
    Label2: TLabel;
    lbError: TLabel;
    Button1: TButton;
    Label3: TLabel;
    cbCounterGood: TCheckBox;
    edCounter: TEdit;
    GroupBox2: TGroupBox;
    cbAutoConnected: TCheckBox;
    Label4: TLabel;
    cbCharging: TComboBox;
    cbAutoMode: TCheckBox;
    Label5: TLabel;
    Label6: TLabel;
    Label7: TLabel;
    edCapacity: TEdit;
    edLevel: TEdit;
    Label8: TLabel;
    pbCharging: TProgressBar;
    btZero: TButton;
    btFull: TButton;
    procedure swTurnClick(Sender: TObject);
    procedure btChangeStatusClick(Sender: TObject);
    procedure cbCounterGoodClick(Sender: TObject);
    procedure cbAutoConnectedClick(Sender: TObject);
    procedure edLevelChange(Sender: TObject);
    procedure btZeroClick(Sender: TObject);
    procedure btFullClick(Sender: TObject);
  private
    { Private declarations }
    fChannel: TChannel;
    FAutomobile: TAutomobile;
    procedure RecalcHeight;
    procedure UpdateControls;
    procedure OnStatusChanged(newState: Integer);
    procedure OnMeterValueChanged(value: Integer);
    procedure OnResistorStateChanged(newState: Integer);
    procedure OnChargeLevelChanged(newLevel: Integer);
  public
    procedure SetupChannel(channel: TChannel);
    procedure SetCaption(Cap : string);
    procedure InitAutomobile;
  end;

implementation

uses
  Common, ChangeStatus, VehicleConfig;

{$R *.dfm}

procedure TIdemFram.SetCaption(Cap: string);
begin
  gbMain.Caption := Cap;
end;

procedure TIdemFram.swTurnClick(Sender: TObject);
begin
  fChannel.setState(swTurn.StateOn);
  UpdateControls;
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

procedure TIdemFram.InitAutomobile;
var
  vehicleConfig: TVehicleConfig;
  Capacity: Integer;
begin
  Capacity := 120;
  vehicleConfig := vehicleConfigList.getConfigById(fChannel.VehicleConfigId);
  if vehicleConfig <> nil then
    Capacity := vehicleConfig.Capacity;
  FAutomobile := TAutomobile.Create(Capacity);

  edCapacity.Text := IntToStr(FAutomobile.Capacity);
  edLevel.Text := IntToStr(FAutomobile.ChargeLevel);
  pbCharging.Max := FAutomobile.Capacity;
  pbCharging.Step := pbCharging.Max div 20;
  pbCharging.Position := FAutomobile.ChargeLevel;
  cbAutoMode.Checked := FAutomobile.AutoMode;
  cbCharging.Enabled := not FAutomobile.AutoMode;

  FAutomobile.OnResistorStateChanged := OnResistorStateChanged;
  FAutomobile.OnChargeLevelChanged := OnChargeLevelChanged;
end;

procedure TIdemFram.cbAutoConnectedClick(Sender: TObject);
begin
  if cbAutoConnected.Checked then
    FChannel.ConnectAutomobile(FAutomobile)
  else
    FChannel.UnconnectAutomobile;  
end;

procedure TIdemFram.OnResistorStateChanged(newState: Integer);
begin
  cbCharging.ItemIndex := newState;
end;

procedure TIdemFram.OnChargeLevelChanged(newLevel: Integer);
begin
  edLevel.Text := IntToStr(newLevel);
  pbCharging.Position := newLevel;
end;

procedure TIdemFram.edLevelChange(Sender: TObject);
var
  value: Integer;
begin
  Value := StrToIntDef(edLevel.Text, -1);
  if Value >= 0 then
    FAutomobile.ChargeLevel := Value;
end;

procedure TIdemFram.btZeroClick(Sender: TObject);
begin
  //Обнуление уровня заряда батареи
  FAutomobile.ChargeLevel := 0;
end;

procedure TIdemFram.btFullClick(Sender: TObject);
begin
  //Установка полного уровня заряда батареи
  FAutomobile.ChargeLevel := FAutomobile.Capacity;
end;

end.

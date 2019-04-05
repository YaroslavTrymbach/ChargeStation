unit Automobile;

interface

const
  RESISTOR_STATE_B = 0;
  RESISTOR_STATE_C = 1;
  RESISTOR_STATE_D = 2;


type
  TOnEventChargeLevelChanged = procedure (newLevel: Integer) of object;
  TOnEventResistorStateChanged = procedure (newState: Integer) of object;

  TAutomobile = class
  private
    FCapacity: Integer;
    FChargeLevel: Integer;
    FResistorState: Integer;
    FChargingAllow: Boolean;
    FUseFan: Boolean;
    FAutoMode: Boolean;
    FOnChargeLevelChanged: TOnEventChargeLevelChanged;
    FOnResistorStateChanged: TOnEventResistorStateChanged;
    procedure SetChargeLevel(const Value: Integer);
    procedure SetResisorState(const Value: Integer);
    procedure SetChargingAllow(const Value: Boolean);
    function GetIsFullCharge: Boolean;
  public
    constructor Create(Cap: Integer);
    function ConsumeEnergy(Value: Integer): Integer;
    procedure SetResistorToInitState;
    property Capacity: Integer read FCapacity;
    property ChargeLevel: Integer read FChargeLevel write SetChargeLevel;
    property ResistorState: Integer read FResistorState write SetResisorState;
    property ChargingAllow: Boolean read FChargingAllow write SetChargingAllow;
    property AutoMode: Boolean read FAutoMode write FAutoMode;
    property IsFullCharge: Boolean read GetIsFullCharge;
    property OnChargeLevelChanged: TOnEventChargeLevelChanged read FOnChargeLevelChanged write FOnChargeLevelChanged;
    property OnResistorStateChanged: TOnEventResistorStateChanged read FOnResistorStateChanged write FOnResistorStateChanged;
  end;

implementation

{ TAutomobile }

function TAutomobile.ConsumeEnergy(Value: Integer): Integer;
var
  LeftSpace: Integer;
begin
  LeftSpace := Capacity - ChargeLevel;
  if Value > LeftSpace then
    Value := LeftSpace;
  ChargeLevel := ChargeLevel + Value;

  if IsFullCharge then
    ResistorState := RESISTOR_STATE_B;
    
  Result := Value;
end;

constructor TAutomobile.Create(Cap: Integer);
begin
  FCapacity := Cap;
  FResistorState := RESISTOR_STATE_B;
  FUseFan := False;
  FAutoMode := True;
  FChargingAllow := False;
end;

function TAutomobile.GetIsFullCharge: Boolean;
begin
  Result := (FChargeLevel >= FCapacity);
end;

procedure TAutomobile.SetChargeLevel(const Value: Integer);
begin
  if FChargeLevel = Value then
    Exit;

  if Value > FCapacity then
    FChargeLevel := FCapacity
  else if Value < 0 then
    FChargeLevel := 0;
  FChargeLevel := Value;

  if Assigned(FOnChargeLevelChanged) then
    FOnChargeLevelChanged(FChargeLevel);
end;

procedure TAutomobile.SetChargingAllow(const Value: Boolean);
begin
  if(FChargingAllow = Value) then
    Exit;

  FChargingAllow := Value;

  if FChargingAllow then
  begin
    if FAutoMode and not IsFullCharge then
    begin
      if FUseFan then
        ResistorState := RESISTOR_STATE_D
      else
        ResistorState := RESISTOR_STATE_C;
    end;
  end
  else
  begin
    if FAutoMode then
      ResistorState := RESISTOR_STATE_B;
  end;
end;

procedure TAutomobile.SetResisorState(const Value: Integer);
begin
  if FResistorState = Value then
    Exit;

  FResistorState := Value;
  if Assigned(FOnResistorStateChanged) then
    FOnResistorStateChanged(FResistorState);
end;

procedure TAutomobile.SetResistorToInitState;
begin
  ResistorState := RESISTOR_STATE_B;
  FChargingAllow := False;
end;

end.

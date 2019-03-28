unit VehicleConfig;

interface

uses
  Contnrs, SimpleXML;

type
  TVehicleConfig = class
  private
    FId: Integer;
    FCapacity: Integer;
  public
    property Id: Integer read FId;
    property Capacity: Integer read FCapacity;
    constructor Create(Id: Integer; node: IXmlNode);
  end;

  TVehicleConfigList = class(TObjectList)
  public
    procedure Load(node: IXmlNode);
    function getConfigById(id: Integer): TVehicleConfig;
  end;

var
  VehicleConfigList: TVehicleConfigList;

implementation

const
  NodeNameVehicle       = 'Vehicle';
  NodeNameCapacity      = 'Capacity';

  ATTRIB_ID = 'id';

{ TVehicleConfigList }

function TVehicleConfigList.getConfigById(id: Integer): TVehicleConfig;
var
  vehicleConfig: TVehicleConfig;
  I: Integer;
begin
  for I := 0 to Count - 1 do
  begin
    vehicleConfig := Items[I] as TVehicleConfig;
    if vehicleConfig.Id = id then
    begin
      Result := vehicleConfig;
      Exit;
    end;
  end;

  Result := nil;
end;

procedure TVehicleConfigList.Load(node: IXmlNode);
var
  I, id: Integer;
  aElem: IXmlNode;
  vehicleConfig: TVehicleConfig;
begin
  Clear;
  if node = nil then
    Exit;

  for i := 0 to node.ChildNodes.Count - 1 do
  begin
    aElem := node.ChildNodes.Item[i];
    if (aElem.NodeName = NodeNameVehicle) then
    begin
      id := aElem.GetIntAttr(ATTRIB_ID, 0);
      if((id > 0) and (getConfigById(id) = nil)) then
      begin
        vehicleConfig := TVehicleConfig.Create(id, aElem);
        add(vehicleConfig);
      end;
    end;
  end;
end;

{ TVehicleConig }

constructor TVehicleConfig.Create(Id: Integer; node: IXmlNode);
var
  aElem: IXmlNode;
begin
  FId := Id;

  FCapacity := node.Values[NodeNameCapacity];
{  //
  aElem := rootElem.SelectSingleNode(NodeNameCapacity);
  if (aElem<>nil) then
  begin

    //Str := aElem.GetChildText(NodeNameNumber,'');
    //ComPortUst.Port := Str;
  end;}
end;

end.

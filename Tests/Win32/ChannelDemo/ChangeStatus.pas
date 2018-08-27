unit ChangeStatus;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ExtCtrls;

const
  CNANGE_STATE_MODE_STATUS = 0;
  CNANGE_STATE_MODE_ERROR  = 1;

type
  TfmChangeStatus = class(TForm)
    Panel1: TPanel;
    btOK: TButton;
    btCancel: TButton;
    Label1: TLabel;
    Label2: TLabel;
    lbCurrentStatus: TLabel;
    cbNewStatus: TComboBox;
    procedure FormShow(Sender: TObject);
    procedure btOKClick(Sender: TObject);
  private
    fStatus: Integer;
    fMode: Integer;
  public
    property Status: Integer read fStatus write fStatus;
    property Mode: Integer read fMode write fMode;
  end;

implementation

uses
  Channel;

{$R *.dfm}

procedure TfmChangeStatus.FormShow(Sender: TObject);
var
  I: Integer;
begin
  lbCurrentStatus.Caption := ChannelGetStatusString(fStatus);

  cbNewStatus.Clear;

  for I := 0 to CHANNEL_NUMBER_OF_STATUS - 1 do
  begin
    cbNewStatus.Items.Add(ChannelGetStatusString(I));
  end;

  cbNewStatus.ItemIndex := fStatus;
end;

procedure TfmChangeStatus.btOKClick(Sender: TObject);
begin
  fStatus := cbNewStatus.ItemIndex;
end;

end.

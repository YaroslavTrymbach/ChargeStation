unit ChangeStatus;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ExtCtrls;

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
  public
    property Status: Integer read fStatus write fStatus;
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

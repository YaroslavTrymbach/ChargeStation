program ChannelDemo;

uses
  Forms,
  Main in 'Main.pas' {fmMain},
  Common in 'Common.pas',
  DataThreads in 'DataThreads.pas',
  SimpleXML in 'SimpleXML.pas',
  IdemFrame in 'IdemFrame.pas' {IdemFram: TFrame},
  yIniFiles in 'yIniFiles.pas',
  Channel in 'Channel.pas',
  ChangeStatus in 'ChangeStatus.pas' {fmChangeStatus},
  Automobile in 'Automobile.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TfmMain, fmMain);
  Application.Run;
end.

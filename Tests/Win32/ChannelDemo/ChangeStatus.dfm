object fmChangeStatus: TfmChangeStatus
  Left = 366
  Top = 155
  BorderStyle = bsDialog
  Caption = #1048#1079#1084#1077#1085#1077#1085#1080#1077' '#1089#1090#1072#1090#1091#1089#1072
  ClientHeight = 109
  ClientWidth = 278
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 16
  object Label1: TLabel
    Left = 27
    Top = 8
    Width = 49
    Height = 16
    Caption = #1057#1090#1072#1090#1091#1089':'
  end
  object Label2: TLabel
    Left = 16
    Top = 40
    Width = 87
    Height = 16
    Caption = #1048#1079#1084#1077#1085#1080#1090#1100' '#1085#1072':'
  end
  object lbCurrentStatus: TLabel
    Left = 111
    Top = 9
    Width = 106
    Height = 16
    Caption = 'lbCurrentStatus'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Panel1: TPanel
    Left = 0
    Top = 68
    Width = 278
    Height = 41
    Align = alBottom
    TabOrder = 0
    object btOK: TButton
      Left = 32
      Top = 8
      Width = 75
      Height = 25
      Caption = 'OK'
      ModalResult = 1
      TabOrder = 0
      OnClick = btOKClick
    end
    object btCancel: TButton
      Left = 160
      Top = 8
      Width = 75
      Height = 25
      Caption = 'Cancel'
      ModalResult = 2
      TabOrder = 1
    end
  end
  object cbNewStatus: TComboBox
    Left = 112
    Top = 36
    Width = 159
    Height = 24
    ItemHeight = 16
    TabOrder = 1
    Text = 'cbNewStatus'
  end
end

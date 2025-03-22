object EditValue: TEditValue
  Left = 241
  Top = 176
  BorderIcons = [biHelp]
  BorderStyle = bsToolWindow
  Caption = 'Edit Value'
  ClientHeight = 73
  ClientWidth = 179
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Scaled = False
  OnKeyPress = FormKeyPress
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 12
    Width = 30
    Height = 13
    Caption = 'Value:'
  end
  object EditVal: TEdit
    Left = 48
    Top = 8
    Width = 121
    Height = 21
    CharCase = ecUpperCase
    MaxLength = 17
    TabOrder = 0
    Text = '0'
    OnChange = EditValChange
    OnKeyPress = FormKeyPress
  end
  object OK: TButton
    Left = 8
    Top = 40
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    TabOrder = 1
    OnClick = OKClick
  end
  object Cancel: TButton
    Left = 96
    Top = 40
    Width = 75
    Height = 25
    Caption = 'Cancel'
    TabOrder = 2
    OnClick = CancelClick
  end
end

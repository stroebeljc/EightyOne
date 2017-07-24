object ConfigureBreakpoint: TConfigureBreakpoint
  Left = 439
  Top = 126
  BorderIcons = [biHelp]
  BorderStyle = bsToolWindow
  Caption = 'TStates Breakpoint'
  ClientHeight = 105
  ClientWidth = 179
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnKeyPress = FormKeyPress
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 12
    Width = 41
    Height = 13
    Caption = 'Address:'
  end
  object Label2: TLabel
    Left = 8
    Top = 44
    Width = 40
    Height = 13
    Caption = 'T-States'
  end
  object OK: TButton
    Left = 8
    Top = 72
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    TabOrder = 2
    OnClick = OKClick
  end
  object Cancel: TButton
    Left = 96
    Top = 72
    Width = 75
    Height = 25
    Caption = 'Cancel'
    TabOrder = 3
    OnClick = CancelClick
  end
  object EditAddress: TEdit
    Left = 56
    Top = 8
    Width = 113
    Height = 21
    MaxLength = 5
    TabOrder = 0
    Text = '0'
  end
  object EditTStates: TEdit
    Left = 56
    Top = 40
    Width = 113
    Height = 21
    MaxLength = 5
    TabOrder = 1
    Text = '0'
  end
end

object ConfigureBreakpoint: TConfigureBreakpoint
  Left = 281
  Top = 380
  BorderIcons = [biSystemMenu, biMinimize, biMaximize, biHelp]
  BorderStyle = bsToolWindow
  Caption = 'Set '#39'T-States'#39' Breakpoint'
  ClientHeight = 90
  ClientWidth = 194
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
    Left = 64
    Top = 8
    Width = 41
    Height = 13
    Caption = 'Address:'
  end
  object Label2: TLabel
    Left = 128
    Top = 8
    Width = 43
    Height = 13
    Caption = 'T-States:'
  end
  object Label3: TLabel
    Left = 8
    Top = 8
    Width = 47
    Height = 13
    Caption = 'Condition:'
  end
  object OK: TButton
    Left = 16
    Top = 56
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    TabOrder = 2
    OnClick = OKClick
  end
  object Cancel: TButton
    Left = 104
    Top = 56
    Width = 75
    Height = 25
    Caption = 'Cancel'
    TabOrder = 3
    OnClick = CancelClick
  end
  object EditAddress: TEdit
    Left = 64
    Top = 24
    Width = 57
    Height = 21
    MaxLength = 5
    TabOrder = 0
    Text = '0'
  end
  object EditTStates: TEdit
    Left = 128
    Top = 24
    Width = 57
    Height = 21
    MaxLength = 5
    TabOrder = 1
    Text = '0'
  end
  object BreakCondition: TComboBox
    Left = 8
    Top = 24
    Width = 49
    Height = 21
    Style = csDropDownList
    DropDownCount = 5
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ItemHeight = 13
    ParentFont = False
    TabOrder = 4
    Items.Strings = (
      '='
      '<'
      '>'
      '<>'
      '-->')
  end
end

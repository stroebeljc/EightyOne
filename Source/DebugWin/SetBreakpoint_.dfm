object SetBreakpoint: TSetBreakpoint
  Left = 190
  Top = 127
  BorderStyle = bsToolWindow
  Caption = 'Set Breakpoint'
  ClientHeight = 90
  ClientWidth = 331
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
  object LabelBreakAddress: TLabel
    Left = 144
    Top = 8
    Width = 41
    Height = 13
    Caption = 'Address:'
  end
  object LabelConditionValue: TLabel
    Left = 208
    Top = 8
    Width = 47
    Height = 13
    Caption = 'Condition:'
  end
  object LabelBreakValue: TLabel
    Left = 264
    Top = 8
    Width = 30
    Height = 13
    Caption = 'Value:'
  end
  object LabelType: TLabel
    Left = 8
    Top = 8
    Width = 27
    Height = 13
    Caption = 'Type:'
  end
  object LabelConditionAddr: TLabel
    Left = 88
    Top = 8
    Width = 47
    Height = 13
    Caption = 'Condition:'
  end
  object FlagList: TComboBox
    Left = 144
    Top = 24
    Width = 57
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 2
    Visible = False
    Items.Strings = (
      '0: C'
      '1: N'
      '2: P/V'
      '3: -'
      '4: H'
      '5: -'
      '6 :Z'
      '7: S')
  end
  object RegisterList: TComboBox
    Left = 144
    Top = 24
    Width = 57
    Height = 21
    Style = csDropDownList
    DropDownCount = 18
    ItemHeight = 13
    TabOrder = 1
    Visible = False
    OnChange = RegisterListChange
    Items.Strings = (
      'BC'
      'DE'
      'HL'
      'IX'
      'IY'
      'PC'
      'SP'
      'BC'#39
      'DE'#39
      'HL'#39
      'A'
      'B'
      'C'
      'D'
      'E'
      'H'
      'L'
      'I'
      'R'
      'IXh'
      'IXl'
      'IYh'
      'IYl'
      'A'#39
      'B'#39
      'C'#39
      'D'#39
      'E'#39
      'H'#39
      'L'#39)
  end
  object OK: TButton
    Left = 88
    Top = 56
    Width = 73
    Height = 25
    Caption = 'OK'
    Default = True
    TabOrder = 5
    OnClick = OKClick
  end
  object Cancel: TButton
    Left = 168
    Top = 56
    Width = 73
    Height = 25
    Caption = 'Cancel'
    TabOrder = 6
    OnClick = CancelClick
  end
  object BreakAddress: TEdit
    Left = 144
    Top = 24
    Width = 57
    Height = 21
    CharCase = ecUpperCase
    MaxLength = 5
    TabOrder = 0
    Text = '0'
    OnChange = BreakAddressChange
  end
  object BreakValue: TEdit
    Left = 264
    Top = 24
    Width = 57
    Height = 21
    CharCase = ecUpperCase
    MaxLength = 5
    TabOrder = 4
    Text = '0'
    OnChange = BreakValueChange
  end
  object BreakConditionAddr: TComboBox
    Left = 88
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
    TabOrder = 8
    OnChange = BreakConditionAddrChange
  end
  object BreakType: TComboBox
    Left = 8
    Top = 24
    Width = 73
    Height = 21
    Style = csDropDownList
    DropDownCount = 13
    ItemHeight = 13
    TabOrder = 7
    OnChange = BreakTypeChange
    Items.Strings = (
      'Execute'
      'Read'
      'Write'
      'Out A15-0'
      'Out A15-8'
      'Out A7-0'
      'In A15-0'
      'In A15-8'
      'In A7-0'
      'Clock '
      'Memory'
      'Register'
      'Flag')
  end
  object BreakConditionValue: TComboBox
    Left = 208
    Top = 24
    Width = 49
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 3
  end
end

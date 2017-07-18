object EditGeneralForm: TEditGeneralForm
  Left = 994
  Top = 211
  Width = 640
  Height = 664
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSizeToolWin
  Caption = 'Edit General Data Block'
  Color = clBtnFace
  Constraints.MinHeight = 377
  Constraints.MinWidth = 632
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label4: TLabel
    Left = 143
    Top = 7
    Width = 116
    Height = 13
    Caption = 'Pilot/Sync Symbol Table'
  end
  object Label5: TLabel
    Left = 143
    Top = 103
    Width = 90
    Height = 13
    Caption = 'Data Symbol Table'
  end
  object Label7: TLabel
    Left = 6
    Top = 605
    Width = 33
    Height = 13
    Anchors = [akLeft, akBottom]
    Caption = 'Pause:'
  end
  object Label8: TLabel
    Left = 7
    Top = 207
    Width = 23
    Height = 13
    Caption = 'Data'
  end
  object Label9: TLabel
    Left = 295
    Top = 605
    Width = 68
    Height = 13
    Anchors = [akLeft, akBottom]
    Caption = 'Character Set:'
  end
  object Label1: TLabel
    Left = 7
    Top = 7
    Width = 75
    Height = 13
    Caption = 'Pilot/Sync Data'
  end
  object SymDefP: TMemo
    Left = 143
    Top = 23
    Width = 479
    Height = 73
    Anchors = [akLeft, akTop, akRight]
    Lines.Strings = (
      'SyncPulses')
    ReadOnly = True
    ScrollBars = ssBoth
    TabOrder = 1
  end
  object SymDefD: TMemo
    Left = 143
    Top = 119
    Width = 479
    Height = 81
    Anchors = [akLeft, akTop, akRight]
    Lines.Strings = (
      'Alphabet')
    ReadOnly = True
    ScrollBars = ssBoth
    TabOrder = 2
  end
  object Pause: TEdit
    Left = 46
    Top = 601
    Width = 144
    Height = 21
    Anchors = [akLeft, akBottom]
    TabOrder = 4
    Text = 'Pause'
  end
  object OK: TButton
    Left = 551
    Top = 598
    Width = 74
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'OK'
    Default = True
    TabOrder = 6
    OnClick = OKClick
  end
  object Data: TMemo
    Left = 7
    Top = 223
    Width = 616
    Height = 363
    Anchors = [akLeft, akTop, akRight, akBottom]
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Fixedsys'
    Font.Style = []
    Lines.Strings = (
      'Data')
    ParentFont = False
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 3
  end
  object CharSet: TComboBox
    Left = 369
    Top = 601
    Width = 88
    Height = 21
    Style = csDropDownList
    Anchors = [akLeft, akBottom]
    ItemHeight = 13
    TabOrder = 5
    OnChange = CharSetChange
    Items.Strings = (
      'ZX81'
      'ASCII')
  end
  object PRLE: TMemo
    Left = 7
    Top = 23
    Width = 128
    Height = 177
    Lines.Strings = (
      'PRLE')
    ScrollBars = ssVertical
    TabOrder = 0
  end
  object Button1: TButton
    Left = 466
    Top = 597
    Width = 75
    Height = 25
    Caption = 'Button1'
    TabOrder = 7
  end
end

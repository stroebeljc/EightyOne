object FSSettings: TFSSettings
  Left = 189
  Top = 81
  BorderIcons = []
  BorderStyle = bsToolWindow
  Caption = 'Display Settings'
  ClientHeight = 214
  ClientWidth = 192
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Scaled = False
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label2: TLabel
    Left = 8
    Top = 8
    Width = 47
    Height = 13
    Caption = 'Renderer:'
  end
  object OK: TButton
    Left = 110
    Top = 183
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 3
    OnClick = OKClick
  end
  object GroupBox1: TGroupBox
    Left = 8
    Top = 48
    Width = 177
    Height = 129
    Caption = 'Full Screen Settings'
    TabOrder = 2
    object Label1: TLabel
      Left = 8
      Top = 24
      Width = 53
      Height = 13
      Caption = 'Resolution:'
    end
    object ModeList: TComboBox
      Left = 8
      Top = 40
      Width = 161
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 0
      OnChange = ModeListChange
    end
    object Letterbox: TRadioButton
      Left = 8
      Top = 88
      Width = 97
      Height = 17
      Caption = 'Letterbox'
      TabOrder = 2
      OnClick = ModeListChange
    end
    object Stretch: TRadioButton
      Left = 8
      Top = 72
      Width = 97
      Height = 17
      Caption = 'Stretch Image'
      Checked = True
      TabOrder = 1
      TabStop = True
      OnClick = ModeListChange
    end
    object White: TCheckBox
      Left = 8
      Top = 104
      Width = 97
      Height = 17
      Caption = 'White Letterbox'
      TabOrder = 3
      OnClick = ModeListChange
    end
  end
  object GDIBtn: TRadioButton
    Left = 24
    Top = 24
    Width = 65
    Height = 17
    Caption = 'GDI'
    Checked = True
    TabOrder = 0
    TabStop = True
    OnClick = GDIBtnClick
  end
  object DDrawBtn: TRadioButton
    Left = 96
    Top = 24
    Width = 89
    Height = 17
    Caption = 'DirectDraw'
    TabOrder = 1
    OnClick = GDIBtnClick
  end
end

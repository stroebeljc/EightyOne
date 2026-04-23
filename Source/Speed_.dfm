object Speed: TSpeed
  Left = 189
  Top = 227
  BorderIcons = []
  BorderStyle = bsToolWindow
  Caption = 'Speed'
  ClientHeight = 110
  ClientWidth = 369
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
  PixelsPerInch = 96
  TextHeight = 13
  object AccuratePanel: TPanel
    Left = 0
    Top = 0
    Width = 369
    Height = 73
    BevelOuter = bvNone
    TabOrder = 1
    object Label9: TLabel
      Left = 8
      Top = 4
      Width = 34
      Height = 13
      Caption = 'Speed:'
    end
    object Label10: TLabel
      Left = 12
      Top = 48
      Width = 11
      Height = 13
      Caption = '0x'
    end
    object Label11: TLabel
      Left = 156
      Top = 48
      Width = 20
      Height = 13
      Caption = '1.0x'
    end
    object Label12: TLabel
      Left = 336
      Top = 48
      Width = 20
      Height = 13
      Caption = '4.0x'
    end
    object Label1: TLabel
      Left = 40
      Top = 48
      Width = 26
      Height = 13
      Caption = '0.25x'
    end
    object Label2: TLabel
      Left = 80
      Top = 48
      Width = 20
      Height = 13
      Caption = '0.5x'
    end
    object Label3: TLabel
      Left = 120
      Top = 48
      Width = 26
      Height = 13
      Caption = '0.75x'
    end
    object Label4: TLabel
      Left = 192
      Top = 48
      Width = 26
      Height = 13
      Caption = '1.25x'
    end
    object Label5: TLabel
      Left = 232
      Top = 48
      Width = 20
      Height = 13
      Caption = '1.5x'
    end
    object Label6: TLabel
      Left = 264
      Top = 48
      Width = 26
      Height = 13
      Caption = '1.75x'
    end
    object Label7: TLabel
      Left = 304
      Top = 48
      Width = 20
      Height = 13
      Caption = '2.0x'
    end
    object AccurateSpeed: TTrackBar
      Left = 8
      Top = 16
      Width = 353
      Height = 25
      Max = 9
      Orientation = trHorizontal
      Frequency = 1
      Position = 4
      SelEnd = 0
      SelStart = 0
      TabOrder = 0
      TickMarks = tmBottomRight
      TickStyle = tsAuto
      OnChange = Recalc
    end
  end
  object Button1: TButton
    Left = 280
    Top = 80
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    TabOrder = 2
    OnClick = Button1Click
  end
  object EnableFrameSkip: TCheckBox
    Left = 8
    Top = 84
    Width = 113
    Height = 17
    Caption = 'Enable Frame Skip'
    TabOrder = 0
    OnClick = EnableFrameSkipClick
  end
end

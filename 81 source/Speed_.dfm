object Speed: TSpeed
  Left = 349
  Top = 187
  BorderStyle = bsDialog
  Caption = 'Speed'
  ClientHeight = 167
  ClientWidth = 282
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object AccuratePanel: TPanel
    Left = 0
    Top = 24
    Width = 281
    Height = 73
    BevelOuter = bvNone
    TabOrder = 1
    object Label9: TLabel
      Left = 32
      Top = 20
      Width = 34
      Height = 13
      Caption = 'Speed:'
    end
    object Label10: TLabel
      Left = 156
      Top = 48
      Width = 33
      Height = 13
      Caption = 'Normal'
    end
    object Label11: TLabel
      Left = 72
      Top = 48
      Width = 26
      Height = 13
      Caption = '1/8th'
    end
    object Label12: TLabel
      Left = 256
      Top = 48
      Width = 11
      Height = 13
      Caption = '8x'
    end
    object AccurateSpeed: TTrackBar
      Left = 64
      Top = 16
      Width = 201
      Height = 25
      Max = 7
      Min = 1
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
  object FastPanel: TPanel
    Left = 0
    Top = 0
    Width = 273
    Height = 129
    BevelOuter = bvNone
    TabOrder = 2
    object Label1: TLabel
      Left = 8
      Top = 20
      Width = 53
      Height = 13
      Caption = 'Fast Mode:'
    end
    object Label2: TLabel
      Left = 8
      Top = 84
      Width = 56
      Height = 13
      Caption = 'Slow Mode:'
    end
    object Label3: TLabel
      Left = 148
      Top = 48
      Width = 33
      Height = 13
      Caption = 'Normal'
    end
    object Label4: TLabel
      Left = 72
      Top = 48
      Width = 32
      Height = 13
      Caption = '1/16th'
    end
    object Label5: TLabel
      Left = 256
      Top = 48
      Width = 17
      Height = 13
      Caption = '16x'
    end
    object Label6: TLabel
      Left = 148
      Top = 112
      Width = 33
      Height = 13
      Caption = 'Normal'
    end
    object Label7: TLabel
      Left = 72
      Top = 112
      Width = 32
      Height = 13
      Caption = '1/16th'
    end
    object Label8: TLabel
      Left = 256
      Top = 112
      Width = 17
      Height = 13
      Caption = '16x'
    end
    object FastMode: TTrackBar
      Left = 64
      Top = 16
      Width = 201
      Height = 25
      Max = 9
      Min = 1
      Orientation = trHorizontal
      PageSize = 1
      Frequency = 1
      Position = 5
      SelEnd = 0
      SelStart = 0
      TabOrder = 0
      TickMarks = tmBottomRight
      TickStyle = tsAuto
      OnChange = Recalc
    end
    object SlowMode: TTrackBar
      Left = 64
      Top = 80
      Width = 201
      Height = 25
      Max = 9
      Min = 1
      Orientation = trHorizontal
      PageSize = 1
      Frequency = 1
      Position = 5
      SelEnd = 0
      SelStart = 0
      TabOrder = 1
      TickMarks = tmBottomRight
      TickStyle = tsAuto
      OnChange = Recalc
    end
  end
  object Button1: TButton
    Left = 200
    Top = 136
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    TabOrder = 0
    OnClick = Button1Click
  end
end

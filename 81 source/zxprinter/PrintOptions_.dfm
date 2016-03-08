object PrintOptions: TPrintOptions
  Left = 342
  Top = 73
  Width = 281
  Height = 180
  Caption = 'Print Options'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object BitRateTxt: TLabel
    Left = 8
    Top = 8
    Width = 73
    Height = 13
    Caption = 'Carriage Speed'
  end
  object LineRateTxt: TLabel
    Left = 8
    Top = 64
    Width = 89
    Height = 13
    Caption = 'Paper Feed Speed'
  end
  object BitRate: TTrackBar
    Left = 0
    Top = 24
    Width = 273
    Height = 33
    Max = 100
    Orientation = trHorizontal
    Frequency = 5
    Position = 0
    SelEnd = 0
    SelStart = 0
    TabOrder = 0
    TickMarks = tmBottomRight
    TickStyle = tsAuto
    OnChange = BitRateChange
  end
  object LineRate: TTrackBar
    Left = 0
    Top = 80
    Width = 273
    Height = 33
    Max = 100
    Orientation = trHorizontal
    Frequency = 5
    Position = 0
    SelEnd = 0
    SelStart = 0
    TabOrder = 1
    TickMarks = tmBottomRight
    TickStyle = tsAuto
    OnChange = LineRateChange
  end
  object OK: TButton
    Left = 96
    Top = 120
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 2
    OnClick = OKClick
  end
end

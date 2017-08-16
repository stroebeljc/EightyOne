object LiveMemoryWindow: TLiveMemoryWindow
  Left = 172
  Top = 219
  BorderStyle = bsSingle
  Caption = 'Live Memory'
  ClientHeight = 275
  ClientWidth = 512
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  OnMouseMove = FormMouseMove
  OnPaint = FormPaint
  PixelsPerInch = 96
  TextHeight = 13
  object StatusBar1: TStatusBar
    Left = 0
    Top = 256
    Width = 512
    Height = 19
    Panels = <
      item
        Alignment = taCenter
        Text = '$0000'
        Width = 50
      end
      item
        Alignment = taCenter
        Text = '0 - 64K'
        Width = 75
      end
      item
        Bevel = pbNone
        Width = 50
      end>
    SimplePanel = False
  end
  object TrackBar1: TTrackBar
    Left = 152
    Top = 272
    Width = 150
    Height = 45
    Orientation = trHorizontal
    Frequency = 1
    Position = 0
    SelEnd = 0
    SelStart = 0
    TabOrder = 1
    TickMarks = tmBottomRight
    TickStyle = tsAuto
  end
  object MainMenu1: TMainMenu
    Left = 16
    Top = 24
    object Re1: TMenuItem
      Caption = '&Edit'
      object Reset1: TMenuItem
        Caption = '&Reset'
        OnClick = Reset1Click
      end
    end
    object View1: TMenuItem
      Caption = '&View'
      object Reads1: TMenuItem
        Caption = '&Reads'
        Checked = True
        OnClick = Reads1Click
      end
      object Writes1: TMenuItem
        Caption = '&Writes'
        Checked = True
        OnClick = Writes1Click
      end
      object Touches1: TMenuItem
        Caption = '&Mark Touched'
        Checked = True
        OnClick = Touches1Click
      end
    end
    object Zoom1: TMenuItem
      Caption = '&Zoom'
      object ROM1: TMenuItem
        Tag = 2048
        Caption = '&ROM'
        OnClick = ROM1Click
      end
      object N1632K1: TMenuItem
        Tag = 8208
        Caption = '16 - 32K'
        OnClick = ROM1Click
      end
      object N816K1: TMenuItem
        Tag = 4104
        Caption = '8 - 16K'
        OnClick = ROM1Click
      end
      object N848K1: TMenuItem
        Tag = 12296
        Caption = '8 - 48K'
        OnClick = ROM1Click
      end
      object WholeMemory1: TMenuItem
        Tag = 16384
        Caption = '&Whole Memory'
        OnClick = ROM1Click
      end
    end
  end
end

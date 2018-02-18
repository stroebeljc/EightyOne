object LiveMemoryWindow: TLiveMemoryWindow
  Left = 393
  Top = 153
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsToolWindow
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
  OnClose = FormClose
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
  object MainMenu1: TMainMenu
    Left = 16
    Top = 24
    object Re1: TMenuItem
      Caption = '&Display'
      object Reset1: TMenuItem
        Caption = '&Reset'
        OnClick = Reset1Click
      end
      object Touches1: TMenuItem
        Caption = '&Show Touched Bytes'
        OnClick = Touches1Click
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
    end
    object Zoom1: TMenuItem
      Caption = '&Range'
      object ROM1: TMenuItem
        Tag = 2048
        Caption = '0 - 8K'
        OnClick = ROM1Click
      end
      object N032: TMenuItem
        Tag = 8192
        Caption = '0 - 32K'
        OnClick = ROM1Click
      end
      object WholeMemory1: TMenuItem
        Tag = 16384
        Caption = '0 - 64K'
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
      object N1632K1: TMenuItem
        Tag = 8208
        Caption = '16 - 32K'
        OnClick = ROM1Click
      end
    end
  end
end

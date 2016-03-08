object SymbolBrowser: TSymbolBrowser
  Left = 334
  Top = 139
  Width = 282
  Height = 429
  BorderStyle = bsSizeToolWin
  Caption = 'Symbol Browser'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object StatusBar1: TStatusBar
    Left = 0
    Top = 383
    Width = 274
    Height = 19
    Panels = <>
    SimplePanel = False
  end
  object ListBox1: TListBox
    Left = 0
    Top = 0
    Width = 274
    Height = 380
    Anchors = [akLeft, akTop, akRight, akBottom]
    BiDiMode = bdRightToLeft
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 14
    ParentBiDiMode = False
    ParentFont = False
    PopupMenu = Dbg.MemDumpPopup
    Sorted = True
    TabOrder = 1
    OnContextPopup = ListBox1ContextPopup
  end
end

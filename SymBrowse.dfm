object SymbolBrowser: TSymbolBrowser
  Left = 329
  Top = 130
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
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object ListBox1: TListBox
    Left = 0
    Top = 0
    Width = 274
    Height = 393
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
    TabOrder = 0
    OnContextPopup = ListBox1ContextPopup
    OnDblClick = ListBox1DblClick
    OnKeyPress = ListBox1KeyPress
  end
end

object Printer: TPrinter
  Left = 508
  Top = 116
  Anchors = [akLeft, akTop, akBottom]
  BorderStyle = bsDialog
  Caption = 'Printer'
  ClientHeight = 301
  ClientWidth = 273
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object DisplayImage: TImage
    Left = 0
    Top = 0
    Width = 256
    Height = 257
    Anchors = [akLeft, akTop, akRight]
  end
  object ScrollBar: TScrollBar
    Left = 257
    Top = 0
    Width = 16
    Height = 258
    Anchors = [akTop, akRight]
    Kind = sbVertical
    PageSize = 0
    TabOrder = 1
    OnChange = ScrollBarChange
  end
  object SaveImage: TButton
    Left = 8
    Top = 264
    Width = 75
    Height = 25
    Caption = 'Save Image'
    TabOrder = 2
    OnClick = SaveImageClick
  end
  object ClearImage: TButton
    Left = 100
    Top = 264
    Width = 75
    Height = 25
    Caption = 'Clear Image'
    TabOrder = 0
    OnClick = ClearImageClick
  end
  object OptionsBtn: TButton
    Left = 192
    Top = 264
    Width = 75
    Height = 25
    Caption = 'Options'
    TabOrder = 3
    OnClick = OptionsBtnClick
  end
  object SaveDialog: TSaveDialog
    DefaultExt = 'bmp'
    Filter = 'Windows Bitmap (BMP)|*.bmp'
    Left = 8
    Top = 8
  end
end

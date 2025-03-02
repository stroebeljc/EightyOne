object MemoryWindow: TMemoryWindow
  Left = 197
  Top = 159
  Width = 580
  Height = 302
  HorzScrollBar.Visible = False
  VertScrollBar.Visible = False
  Anchors = [akLeft, akTop, akRight, akBottom]
  BorderStyle = bsSizeToolWin
  Caption = 'Memory'
  Color = clBtnFace
  Constraints.MinHeight = 100
  Constraints.MinWidth = 180
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Courier New'
  Font.Style = []
  FormStyle = fsStayOnTop
  KeyPreview = True
  OldCreateOrder = False
  PopupMenu = PopupMenu1
  Scaled = False
  OnClick = FormClick
  OnKeyDown = FormKeyDown
  OnMouseDown = FormMouseDown
  OnMouseMove = FormMouseMove
  OnMouseWheel = FormMouseWheel
  OnPaint = FormPaint
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 14
  object ScrollBar1: TScrollBar
    Left = 547
    Top = 0
    Width = 17
    Height = 245
    Align = alRight
    Kind = sbVertical
    LargeChange = 8
    Max = 65535
    PageSize = 0
    TabOrder = 0
    OnChange = ScrollBar1Change
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 245
    Width = 564
    Height = 19
    Panels = <
      item
        Width = 25
      end
      item
        Width = 25
      end
      item
        Width = 25
      end
      item
        Width = 25
      end
      item
        Width = 40
      end>
    SimplePanel = False
  end
  object ButtonFirstChange: TButton
    Left = 0
    Top = 224
    Width = 40
    Height = 19
    Caption = '| <'
    TabOrder = 2
    OnClick = ButtonFirstChangeClick
    OnMouseMove = ButtonChangeMouseMove
  end
  object ButtonPrevChange: TButton
    Tag = 1
    Left = 48
    Top = 224
    Width = 40
    Height = 19
    Caption = '<'
    TabOrder = 3
    OnClick = ButtonPrevChangeClick
    OnMouseMove = ButtonChangeMouseMove
  end
  object ButtonNextChange: TButton
    Tag = 2
    Left = 96
    Top = 224
    Width = 40
    Height = 19
    Caption = '>'
    TabOrder = 4
    OnClick = ButtonNextChangeClick
    OnMouseMove = ButtonChangeMouseMove
  end
  object ButtonLastChange: TButton
    Tag = 3
    Left = 144
    Top = 224
    Width = 40
    Height = 19
    Caption = '> |'
    TabOrder = 5
    OnClick = ButtonLastChangeClick
    OnMouseMove = ButtonChangeMouseMove
  end
  object IncDecAddress: TUpDown
    Left = 0
    Top = 0
    Width = 30
    Height = 17
    Min = -32768
    Max = 32767
    Orientation = udHorizontal
    ParentShowHint = False
    Position = 0
    ShowHint = False
    TabOrder = 6
    TabStop = True
    Wrap = False
    OnChangingEx = IncDecAddressChangingEx
  end
  object PopupMenu1: TPopupMenu
    Left = 8
    Top = 160
    object ViewTraditional1: TMenuItem
      Tag = 4
      Caption = 'View Hex Bytes and Characters'
      Checked = True
      OnClick = ViewBytes1Click
    end
    object ViewBytes1: TMenuItem
      Caption = 'View Hex Bytes'
      OnClick = ViewBytes1Click
    end
    object ViewWords1: TMenuItem
      Tag = 1
      Caption = 'View Hex Words'
      OnClick = ViewBytes1Click
    end
    object ViewBinary1: TMenuItem
      Tag = 2
      Caption = 'View Binary Bytes'
      OnClick = ViewBytes1Click
    end
    object ViewDecimal1: TMenuItem
      Tag = 3
      Caption = 'View Decimal Bytes'
      OnClick = ViewBytes1Click
    end
    object SetAddress1: TMenuItem
      Caption = 'Jump To ...'
      OnClick = SetAddress1Click
    end
    object JumpTo1: TMenuItem
      Caption = 'Jump To'
      Enabled = False
      OnClick = JumpTo1Click
    end
    object Search1: TMenuItem
      Caption = 'Find...'
      OnClick = Search1Click
    end
    object ClearHighlights: TMenuItem
      Caption = 'Clear Highlights'
      OnClick = ClearHighlightsClick
    end
  end
end

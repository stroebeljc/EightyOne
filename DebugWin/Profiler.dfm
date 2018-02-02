object Profiler: TProfiler
  Left = 214
  Top = 306
  Width = 349
  Height = 358
  Caption = 'Profiler'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object ButtonNew: TButton
    Left = 8
    Top = 291
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'New'
    TabOrder = 0
    OnClick = ButtonNewClick
  end
  object ButtonDelete: TButton
    Left = 248
    Top = 291
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'Delete'
    TabOrder = 1
  end
  object ListViewProfileSamples: TListView
    Left = 8
    Top = 8
    Width = 315
    Height = 276
    Anchors = [akLeft, akTop, akRight, akBottom]
    Columns = <
      item
        Caption = 'Tag'
      end
      item
        Caption = 'Start'
      end
      item
        Caption = 'End'
      end
      item
        Caption = 'Max'
      end
      item
        Caption = 'Samples'
      end>
    ColumnClick = False
    GridLines = True
    RowSelect = True
    TabOrder = 2
    ViewStyle = vsReport
  end
  object ButtonEdit: TButton
    Left = 88
    Top = 291
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'Edit'
    TabOrder = 3
    OnClick = ButtonEditClick
  end
  object ButtonReset: TButton
    Left = 168
    Top = 291
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'Reset'
    TabOrder = 4
    OnClick = ButtonResetClick
  end
end

object Profiler: TProfiler
  Left = 250
  Top = 255
  Width = 459
  Height = 359
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
    Left = 4
    Top = 292
    Width = 75
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'New'
    TabOrder = 0
    OnClick = ButtonNewClick
  end
  object ButtonDelete: TButton
    Left = 244
    Top = 292
    Width = 75
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'Delete'
    TabOrder = 1
    OnClick = ButtonDeleteClick
  end
  object ListViewProfileSamples: TListView
    Left = 8
    Top = 8
    Width = 425
    Height = 277
    Anchors = [akLeft, akTop, akRight, akBottom]
    Columns = <
      item
        Caption = 'Tag'
        Width = 100
      end
      item
        Caption = 'Start'
      end
      item
        Caption = 'End'
      end
      item
        Caption = 'Min T'
        Width = 60
      end
      item
        Caption = 'Max T'
        Width = 60
      end
      item
        Caption = 'Samples'
        Width = 100
      end>
    ColumnClick = False
    GridLines = True
    RowSelect = True
    TabOrder = 2
    ViewStyle = vsReport
  end
  object ButtonEdit: TButton
    Left = 84
    Top = 292
    Width = 75
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'Edit'
    TabOrder = 3
    OnClick = ButtonEditClick
  end
  object ButtonReset: TButton
    Left = 164
    Top = 292
    Width = 75
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'Reset'
    TabOrder = 4
    OnClick = ButtonResetClick
  end
end

object WavLoad: TWavLoad
  Left = 508
  Top = 108
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsDialog
  Caption = 'Wave Loader'
  ClientHeight = 233
  ClientWidth = 401
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
  object Image1: TImage
    Left = 0
    Top = 0
    Width = 401
    Height = 129
    OnMouseMove = Image1MouseMove
  end
  object Status: TLabel
    Left = 0
    Top = 216
    Width = 40
    Height = 13
    Caption = 'Stopped'
  end
  object StereoTxt1: TLabel
    Left = 208
    Top = 208
    Width = 19
    Height = 13
    Caption = 'Use'
  end
  object StereoTxt2: TLabel
    Left = 288
    Top = 208
    Width = 39
    Height = 13
    Caption = 'Channel'
  end
  object ScrollBar: TScrollBar
    Left = 0
    Top = 128
    Width = 401
    Height = 17
    LargeChange = 256
    PageSize = 0
    SmallChange = 4
    TabOrder = 1
    OnChange = ScrollBarChange
  end
  object Load: TButton
    Left = 8
    Top = 160
    Width = 75
    Height = 25
    Caption = 'Open...'
    TabOrder = 2
    OnClick = LoadClick
  end
  object PlayBtn: TButton
    Left = 208
    Top = 160
    Width = 75
    Height = 25
    Caption = 'Play'
    TabOrder = 4
    OnClick = PlayBtnClick
  end
  object SaveBtn: TButton
    Left = 88
    Top = 160
    Width = 75
    Height = 25
    Caption = 'Save...'
    Enabled = False
    TabOrder = 3
  end
  object OKBtn: TButton
    Left = 320
    Top = 160
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    TabOrder = 0
    OnClick = OKBtnClick
  end
  object AutoBtn: TCheckBox
    Left = 88
    Top = 208
    Width = 97
    Height = 17
    Caption = 'Auto Start/Stop'
    TabOrder = 5
  end
  object LeftBtn: TRadioButton
    Left = 240
    Top = 200
    Width = 41
    Height = 17
    Caption = 'Left'
    TabOrder = 6
    OnClick = LeftBtnClick
  end
  object RightBtn: TRadioButton
    Left = 240
    Top = 216
    Width = 49
    Height = 17
    Caption = 'Right'
    TabOrder = 7
    OnClick = RightBtnClick
  end
  object LoadWavDialog: TOpenDialog
    DefaultExt = 'wav'
    Filter = 'WAV Files|*.wav'
    Left = 176
    Top = 160
  end
end

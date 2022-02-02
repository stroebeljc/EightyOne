object MidiForm: TMidiForm
  Left = 242
  Top = 124
  BorderIcons = []
  BorderStyle = bsToolWindow
  Caption = 'Sound Settings'
  ClientHeight = 256
  ClientWidth = 344
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Scaled = False
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 8
    Top = 182
    Width = 160
    Height = 65
    Anchors = []
    Caption = 'Send MIDI Output to:'
    TabOrder = 1
    object MidiBox: TComboBox
      Left = 16
      Top = 24
      Width = 128
      Height = 21
      Style = csDropDownList
      Anchors = [akLeft, akRight]
      ItemHeight = 13
      TabOrder = 0
      OnChange = MidiChange
    end
  end
  object OK: TButton
    Left = 229
    Top = 206
    Width = 75
    Height = 25
    Anchors = []
    Caption = 'OK'
    TabOrder = 2
    OnClick = OKClick
  end
  object GroupBox2: TGroupBox
    Left = 8
    Top = 8
    Width = 160
    Height = 169
    Caption = 'Volume'
    TabOrder = 0
    object Label1: TLabel
      Left = 32
      Top = 24
      Width = 56
      Height = 13
      Caption = 'AY Channel'
    end
    object Label2: TLabel
      Left = 104
      Top = 32
      Width = 34
      Height = 13
      Caption = 'Beeper'
    end
    object Label3: TLabel
      Left = 24
      Top = 40
      Width = 7
      Height = 13
      Caption = 'A'
    end
    object Label4: TLabel
      Left = 56
      Top = 40
      Width = 7
      Height = 13
      Caption = 'B'
    end
    object Label5: TLabel
      Left = 88
      Top = 40
      Width = 7
      Height = 13
      Caption = 'C'
    end
    object Label6: TLabel
      Left = 76
      Top = 146
      Width = 24
      Height = 13
      Caption = 'Mute'
    end
    object ChAVol: TTrackBar
      Left = 16
      Top = 56
      Width = 41
      Height = 89
      Max = 31
      Orientation = trVertical
      Frequency = 16
      Position = 0
      SelEnd = 0
      SelStart = 0
      TabOrder = 0
      TickMarks = tmBottomRight
      TickStyle = tsAuto
      OnChange = MidiChange
    end
    object ChBVol: TTrackBar
      Left = 48
      Top = 56
      Width = 41
      Height = 89
      Max = 31
      Orientation = trVertical
      Frequency = 16
      Position = 0
      SelEnd = 0
      SelStart = 0
      TabOrder = 1
      TickMarks = tmBottomRight
      TickStyle = tsAuto
      OnChange = MidiChange
    end
    object ChCVol: TTrackBar
      Left = 80
      Top = 56
      Width = 41
      Height = 89
      Max = 31
      Orientation = trVertical
      Frequency = 16
      Position = 0
      SelEnd = 0
      SelStart = 0
      TabOrder = 2
      TickMarks = tmBottomRight
      TickStyle = tsAuto
      OnChange = MidiChange
    end
    object BeeperVol: TTrackBar
      Left = 112
      Top = 56
      Width = 41
      Height = 89
      Max = 31
      Orientation = trVertical
      Frequency = 16
      Position = 0
      SelEnd = 0
      SelStart = 0
      TabOrder = 3
      TickMarks = tmBottomRight
      TickStyle = tsAuto
      OnChange = MidiChange
    end
    object AYMute: TCheckBox
      Left = 52
      Top = 144
      Width = 17
      Height = 17
      TabOrder = 4
      OnClick = MidiChange
    end
    object BeeperMute: TCheckBox
      Left = 116
      Top = 144
      Width = 17
      Height = 17
      TabOrder = 5
      OnClick = MidiChange
    end
  end
  object GroupBox3: TGroupBox
    Left = 176
    Top = 8
    Width = 161
    Height = 169
    Caption = 'Output Format'
    TabOrder = 3
    object BtnStereo: TCheckBox
      Left = 8
      Top = 72
      Width = 89
      Height = 17
      Caption = 'ACB mix'
      TabOrder = 0
    end
  end
end

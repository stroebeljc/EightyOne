unit OffBtn;

{  Office 97/2000 Assistant style button written by Jonathan Hosking,
   February 2002.

   Get future component updates from the following address
   Website: http://www.the-hoskings.freeserve.co.uk/

   Send any bugs, suggestions, etc to the following Email
   Email: jonathan@the-hoskings.freeserve.co.uk

   Thanks to Kambiz for adding bi-directional support, the auto
   height adjustment routines and fixing some bugs
   Email: khojasteh@mail.com

   Thanks to Michel for fixing a bug in the keyboard routines and
   helping with the auto transparency feature
   Email: michelb@docudatasoft.com

   Thanks to Sherlock for fixing a bug with modal forms
   Email: ShrHolmes@rambler.ru  }

{$IFNDEF VER80} { Not using Delphi 1.0 }
  {$IFNDEF VER90} { Not using Delphi 2.0 }
    {$IFNDEF VER93} { Not using C++Builder 1.0 }
      {$DEFINE OFFBTND3} { Using at least Delphi 3.0 or C++Builder 3.0 }
      {$IFNDEF VER100} { Not using Delphi 3.0 }
        {$IFNDEF VER110} { Not using C++Builder 3.0 }
          {$DEFINE OFFBTND4} { Using at least Delphi 4.0 or C++Builder 4.0 }
        {$ENDIF}
      {$ENDIF}
    {$ENDIF}
  {$ENDIF}
{$ENDIF}

interface

uses
  {$IFDEF WIN32} Windows, {$ELSE} WinTypes, WinProcs, {$ENDIF}
  SysUtils, Messages, Classes, Graphics, Controls, Forms,
  Dialogs, Menus;

type
  TOffBtnAbout = (abNone,abAbout);
  TOffBtnState = (bsInactive, bsActive, bsDown, bsDownAndOut);
  TOffBtnType = (bsButton, bsRadioButton, bsUpButton, bsDownButton, bsHintButton);
  TGlyphPosition = (bsTop, bsBottom, bsLeft, bsRight);
  TOffice97Button = class(TCustomControl)
  private
    { Private declarations }
    fAutoHeight: Boolean;
    fAutoTransparency: Boolean;
    fBtnKey: Boolean;
    fCancel: Boolean;
    fClicksDisabled: Boolean;
    fDefault: Boolean;
    fFocused: Boolean;
    fNoDots: Boolean;
    fOffice2000Look: Boolean;
    fShowGlyph: Boolean;
    fUseCustomGlyphs: Boolean;
    fWordWrap: Boolean;
    fCaption: TCaption;
    fActive: TBitmap;
    fTransparent: Boolean;
    fControl: TBitmap;
    fCustomActive: TBitmap;
    fCustomDisabled: TBitmap;
    fCustomDownActive: TBitmap;
    fCustomInactive: TBitmap;
    fDisabled: TBitmap;
    fDisabledWorking: TBitmap;
    fDownActive: TBitmap;
    fInactive: TBitmap;
    fAbout: TOffBtnAbout;
    fType: TOffBtnType;
    fHoverFont: TFont;
    fMouseExit: TNotifyEvent;
    fMouseEnter: TNotifyEvent;
    fActiveColor: TColor;
    fActiveOutlineColor: TColor;
    fActiveOutlineColor2: TColor;
    fInactiveColor: TColor;
    fTransparentColor: TColor;
    fGlyphPosition: TGlyphPosition;
    fModalResult: TModalResult;
    fDown: boolean;
    fToggle: boolean;
    capWrap: TStringList;
    capLines,tX: Integer;
    procedure DrawTransparentBitmap(Dest:TCanvas;const X,Y:Smallint;srcBmp:TBitmap;const transpColor:TColor);
    procedure CreateDisabledBitmap(FOriginal: TBitmap; OutlineColor: TColor);
    procedure DrawOfficeFocusRect(txtRect: TRect; capWrap: TStringList);
    procedure GetWrapText(txt: String; tX: Integer);
    procedure DrawFrame;
    procedure DrawOffice2000Frame;
    procedure SetAutoHeight(Val: Boolean);
    procedure SetAutoTransparency(Val: Boolean);
    procedure SetCaption(Val: TCaption);
    procedure SetDefault(Value: Boolean);
    function CurrentGlyph: TBitmap;
    procedure SetActiveColor(Val: TColor);
    procedure SetActiveOutlineColor(Val: TColor);
    procedure SetActiveOutlineColor2(Val: TColor);
    procedure SetControlType(Val: TOffBtnType);
    procedure SetCustomActiveGlyph(Val: TBitmap);
    procedure SetCustomDisabledGlyph(Val: TBitmap);
    procedure SetCustomDownActiveGlyph(Val: TBitmap);
    procedure SetCustomInactiveGlyph(Val: TBitmap);
    procedure SetGlyphPosition(Val: TGlyphPosition);
    procedure SetInactiveColor(Val: TColor);
    procedure SetOffice2000Look(Val: Boolean);
    procedure SetShowGlyph(Val: Boolean);
    procedure SetTransparent(Val: Boolean);
    procedure SetTransparentColor(Val: TColor);
    procedure SetUseCustomGlyphs(Val: Boolean);
    procedure SetWordWrap(Val: Boolean);
    procedure SetHoverFont(Val: TFont);
    procedure SetDown(Val: Boolean);
    procedure HoverFontChanged(Sender: TObject);
    procedure ShowAbout(Val: TOffBtnAbout);
    procedure Layout(var txtRect, bitRect: TRect);
    procedure CalculateTxt(var txtRect: TRect;Glyph: TBitmap);
    procedure WMEraseBkgnd(var Message: TWMEraseBkgnd); message wm_EraseBkgnd;
    procedure WMLButtonDown(var Message: TWMLButtonDown); message wm_LButtonDown;
    procedure WMMouseMove(var Message: TWMMouseMove); message wm_MouseMove;
    procedure WMLButtonUp(var Message: TWMLButtonUp); message wm_LButtonUp;
    procedure WMRButtonDown(var Message: TWMRButtonDown); message wm_RButtonDown;
    procedure CNCommand(var Message: TWMCommand); message cn_Command;
    procedure CNKeyDown(var Message: TWMKeyDown); message cn_KeyDown;
    procedure CMDialogChar(var Message: TCMDialogChar); message cm_DialogChar;
    procedure CMDialogKey(var Message: TCMDialogKey); message cm_DialogKey;
    procedure CMEnabledChanged(var Message: TMessage); message cm_EnabledChanged;
    procedure CMFocusChanged(var Message: TMessage); message cm_FocusChanged;
    procedure WMKillFocus(var Message: TWMKillFocus); message wm_KillFocus;
    procedure WMSetFocus(var Message: TWMSetFocus); message wm_SetFocus;
    procedure PerformEraseBackground(Control: TControl; DC: HDC);
  protected
    { Protected declarations }
    fState: TOffBtnState;
    procedure Paint; override;
    procedure WndProc(var Message: TMessage); override;
    procedure CreateWnd; override;
    {$IFDEF OFFBTND4}
    procedure ActionChange(Sender: TObject; CheckDefaults: Boolean); override;
    {$ENDIF}
  public
    { Public declarations }
    constructor Create(AOwner: TComponent); override;
    destructor Destroy; override;
    procedure Click; override;
    procedure AdjustHeight;
  published
    { Published declarations }
    property Office2000Look: Boolean read fOffice2000Look write SetOffice2000Look default True;
    property About: TOffBtnAbout read fAbout write ShowAbout default abNone;
    {$IFDEF OFFBTND4}
    property Action;
    {$ENDIF}
    property ActiveColor: TColor read fActiveColor write SetActiveColor default $00808080;
    property ActiveGlyph: TBitmap read fCustomActive write SetCustomActiveGlyph;
    property ActiveOutlineColor: TColor read fActiveOutlineColor write SetActiveOutlineColor default clWhite;
    property ActiveOutlineColor2: TColor read fActiveOutlineColor2 write SetActiveOutlineColor2 default $00D6E7E7;
    property Align;
    {$IFDEF OFFBTND4}
    property Anchors;
    {$ENDIF}
    property AutoHeight: Boolean read fAutoHeight write SetAutoHeight default False;
    property AutoTransparency: Boolean read fAutoTransparency write SetAutoTransparency default True;
    {$IFDEF OFFBTND4}
    property BiDiMode;
    {$ENDIF}
    property Cancel: Boolean read fCancel write fCancel default False;
    property Caption: TCaption read fCaption write SetCaption;
    property Color;
    property ControlType: TOffBtnType read fType write SetControlType default bsButton;
    property Default: Boolean read fDefault write SetDefault default False;
    property DisabledGlyph: TBitmap read fCustomDisabled write SetCustomDisabledGlyph;
    property DownActiveGlyph: TBitmap read fCustomDownActive write SetCustomDownActiveGlyph;
    property DragCursor;
    property DragMode;
    property Enabled;
    property Font;
    property HoverFont: TFont read fHoverFont write SetHoverFont;
    property InactiveColor: TColor read fInactiveColor write SetInactiveColor default clSilver;
    property InactiveGlyph: TBitmap read fCustomInactive write SetCustomInactiveGlyph;
    property ModalResult: TModalResult read fModalResult write fModalResult default 0;
    {$IFDEF OFFBTND4}
    property ParentBiDiMode;
    {$ENDIF}
    property ParentFont;
    property ParentShowHint;
    property PopupMenu;
    property Position: TGlyphPosition read fGlyphPosition write SetGlyphPosition default bsLeft;
    property ShowGlyph: Boolean read fShowGlyph write SetShowGlyph default False;
    property ShowHint;
    property TabOrder;
    property TabStop default True;
    property Transparent: Boolean read fTransparent write SetTransparent default False;
    property TransparentColor: TColor read fTransparentColor write SetTransparentColor default clBlack;
    property UseCustomGlyphs: Boolean read fUseCustomGlyphs write SetUseCustomGlyphs default False;
    property Visible;
    property WordWrap: Boolean read fWordWrap write SetWordWrap default True;
    property Down: boolean read fDown write SetDown default false;
    property Toggle: boolean read fToggle write fToggle default false;
    property OnClick;
    property OnDblClick;
    property OnDragDrop;
    property OnDragOver;
    property OnEndDrag;
    property OnEnter;
    property OnExit;
    property OnKeyDown;
    property OnKeyPress;
    property OnKeyUp;
    property OnMouseDown;
    property OnMouseExit: TNotifyEvent read fMouseExit write fMouseExit;
    property OnMouseEnter: TNotifyEvent read fMouseEnter write fMouseEnter;
    property OnMouseMove;
    property OnMouseUp;
  end;

procedure Register;

implementation

{ TOffice97Button }

{$IFDEF OFFBTND4}
uses ActnList;
{$ENDIF}

{$R OFFBTN.RES}

const
  CopyRightStr: PChar = 'TOffice97Button Component v2.16 (20/02/2002)'+#13+#13+
    'By Jonathan Hosking'+#13+#13+'Compiled in '+
    {$IFDEF VER80}  'Delphi 1.0' {$ENDIF}
    {$IFDEF VER90}  'Delphi 2.0' {$ENDIF}
    {$IFDEF VER100} 'Delphi 3.0' {$ENDIF}
    {$IFDEF VER120} 'Delphi 4.0' {$ENDIF}
    {$IFDEF VER130} 'Delphi 5.0' {$ENDIF}
    {$IFDEF VER140} 'Delphi 6.0' {$ENDIF}
    {$IFDEF VER93}  'C++Builder 1.0' {$ENDIF}
    {$IFDEF VER110} 'C++Builder 3.0' {$ENDIF}
    {$IFDEF VER125} 'C++Builder 4.0' {$ENDIF};
var
  CopyRightPtr: Pointer;

type
  TParentControl = class(TWinControl);

{ This procedure is exactly copied from RxLibrary VCLUtils. }
procedure CopyParentImage(Control: TControl; Dest: TCanvas);
var
  I, Count, X, Y, SaveIndex: Integer;
  DC: HDC;
  R, SelfR, CtlR: TRect;
begin
  if (Control = nil) or (Control.Parent = nil) then Exit;
  Count := Control.Parent.ControlCount;
  DC := Dest.Handle;
  {$IFDEF WIN32}
  with Control.Parent do ControlState := ControlState + [csPaintCopy];
  try
  {$ENDIF}
    with Control do begin
      SelfR := Bounds(Left, Top, Width, Height);
      X := -Left; Y := -Top;
    end;
    { Copy parent control image }
    SaveIndex := SaveDC(DC);
    try
      SetViewportOrgEx(DC, X, Y, nil);
      IntersectClipRect(DC, 0, 0, Control.Parent.ClientWidth,
        Control.Parent.ClientHeight);
      with TParentControl(Control.Parent) do begin
        Perform(WM_ERASEBKGND, DC, 0);
        PaintWindow(DC);
      end;
    finally
      RestoreDC(DC, SaveIndex);
    end;
    { Copy images of graphic controls }
    for I := 0 to Count - 1 do begin
      if Control.Parent.Controls[I] = Control then Break
      else if (Control.Parent.Controls[I] <> nil) and
        (Control.Parent.Controls[I] is TGraphicControl) then
      begin
        with TGraphicControl(Control.Parent.Controls[I]) do begin
          CtlR := Bounds(Left, Top, Width, Height);
          if Bool(IntersectRect(R, SelfR, CtlR)) and Visible then
          begin
            {$IFDEF WIN32}
            ControlState := ControlState + [csPaintCopy];
            {$ENDIF}
            SaveIndex := SaveDC(DC);
            try
              SetViewportOrgEx(DC, Left + X, Top + Y, nil);
              IntersectClipRect(DC, 0, 0, Width, Height);
              Perform(WM_PAINT, DC, 0);
            finally
              RestoreDC(DC, SaveIndex);
              {$IFDEF WIN32}
              ControlState := ControlState - [csPaintCopy];
              {$ENDIF}
            end;
          end;
        end;
      end;
    end;
  {$IFDEF WIN32}
  finally
    with Control.Parent do ControlState := ControlState - [csPaintCopy];
  end;
  {$ENDIF}
end;

{ This procedure draws a transparent bitmap }
procedure TOffice97Button.DrawTransparentBitmap(Dest:TCanvas;const X,Y:Smallint;srcBmp:TBitmap;const transpColor:TColor);
var
  ANDBitmap,ORBitmap: TBitmap;
  oldCopyMode: TCopyMode;
  src: TRect;
begin
  ANDBitmap := TBitmap.Create;
  ORBitmap := TBitmap.Create;
  try
    Src := Bounds(0,0,srcBmp.Width,srcBmp.Height);
    with ORBitmap do
    begin
      Width := srcBmp.Width;
      Height := srcBmp.Height;
      Canvas.Brush.Color := clBlack;
      Canvas.CopyMode := cmSrcCopy;
      Canvas.BrushCopy(Src,srcBmp,Src,transpColor);
    end;
    with ANDBitmap do
    begin
      Width := srcBmp.Width;
      Height := srcBmp.Height;
      Canvas.Brush.Color := clWhite;
      Canvas.CopyMode := cmSrcInvert;
      Canvas.BrushCopy(Src,srcBmp,Src,transpColor);
    end;
    with Dest do
    begin
      oldCopyMode := CopyMode;
      CopyMode := cmSrcAnd;
      Draw(x,y,ANDBitmap);
      CopyMode := cmSrcPaint;
      Draw(x,y,ORBitmap);
      CopyMode := oldCopyMode;
    end;
  finally
    ORBitmap.Free;
    ANDBitmap.Free;
  end;
end;

{ This procedure creates a "Disabled" style bitmap }
procedure TOffice97Button.CreateDisabledBitmap(FOriginal: TBitmap; OutlineColor: TColor);
const
  ROP_DSPDxax = $00E20746;
var
  MonoBmp: TBitmap;
  IRect: TRect;
  IH,IW: Integer;
begin
  IRect := Rect(0, 0, FOriginal.Width, FOriginal.Height);
  IW := IRect.Right - IRect.Left;
  IH := IRect.Bottom - IRect.Top;
  try
    fDisabledWorking.Width := FOriginal.Width;
    fDisabledWorking.Height := FOriginal.Height;
    MonoBmp := TBitmap.Create;
    try
      with MonoBmp do
      begin
        Width := FOriginal.Width;
        Height := FOriginal.Height;
        Canvas.CopyRect(IRect, FOriginal.Canvas, IRect);
        {$IFDEF VER100}
        HandleType := bmDDB;
        {$ENDIF}
        Canvas.Brush.Color := OutlineColor;
        if Monochrome then
        begin
          Canvas.Font.Color := clWhite;
          Monochrome := False;
          Canvas.Brush.Color := clWhite;
        end;
        Monochrome := True;
      end;
      with fDisabledWorking.Canvas do
      begin
        Brush.Color := clBtnFace;
        FillRect(IRect);
        Brush.Color := clBtnHighlight;
        SetTextColor(Handle, clBlack);
        SetBkColor(Handle, clWhite);
        BitBlt(Handle, 1, 1, IW, IH, MonoBmp.Canvas.Handle, 0, 0, ROP_DSPDxax);
        Brush.Color := clBtnShadow;
        SetTextColor(Handle, clBlack);
        SetBkColor(Handle, clWhite);
        BitBlt(Handle, 0, 0, IW, IH, MonoBmp.Canvas.Handle, 0, 0, ROP_DSPDxax);
      end;
    finally
      MonoBmp.Free;
    end;
  except
    fDisabledWorking.Free;
    raise;
  end;
end;

{ This procedure draws an "Office" style focus

  Thanks to Kambiz for adding bi-directional support to this procedure }
procedure TOffice97Button.DrawOfficeFocusRect(txtRect: TRect; capWrap: TStringList);
var
  tmp,tmp2,x1,x2,y1,y2: Integer;
begin
  x1 := txtRect.Left - 1;
  x2 := txtRect.Right + 2;
  y1 := txtRect.Top - 1;
  y2 := txtRect.Bottom + 1;
  with fControl.Canvas do
  begin
    Pen.Color := Self.Font.Color;
    Pen.Style := psDot;
    Brush.Style := bsClear;
    if (capLines = 1) or (not fWordWrap) then
    begin
      { Since there's only 1 line of text, or no wordwrapping, we use the
        standard focus }
      Rectangle(x1,y1,x2,y2);
      Exit;
    end;
    { Draw our "Office" style focus }
    tmp := x1+2+TextWidth(capWrap.Strings[capLines-1]);
    if tmp > x1+(x2-x1) then tmp := x1+(x2-x1);
    tmp2 := y1+((capLines-1)*TextHeight('0'));
    {$IFDEF OFFBTND4}
    if UseRightToLeftAlignment then
    begin
      tmp := x2-x1-tmp+4;
      PolyLine([Point(x1,tmp2),Point(x1,y1),Point(x2,y1),Point(x2,y2),
        Point(tmp,y2),Point(tmp,tmp2), Point(x1,tmp2)]);
    end
    else
    {$ENDIF}
      PolyLine([Point(x1,y2),Point(x1,y1),Point(x2,y1),Point(x2,tmp2),
        Point(tmp,tmp2),Point(tmp,y2), Point(x1,y2)]);
  end;
end;

{ This procedure divides text into a wordwrapped arrary }
procedure TOffice97Button.GetWrapText(txt: String; tX: Integer);
var
  Count,LastSpace,OCount: Integer;
  tmpTxt: String;
  txtStop: Boolean;
begin
  capLines := 0;
  capWrap.Clear;
  if fControl.Canvas.TextWidth(txt) <= tX then
  begin
    { If just a single line is required, we can skip the loop }
    capLines := 1;
    capWrap.Add(txt);
    Exit;
  end;
  { Chop the line of text into several lines }
  OCount := -1;
  Count := 0;
  while count < length(txt) do
  begin
    if Count = OCount then
    begin
      capLines := 0;
      capWrap.Clear;
      Exit;
    end;
    OCount := Count;
    LastSpace := 0;
    tmpTxt := '';
    txtStop := False;
    while not(txtStop) and (count < length(txt)) do
    begin
      inc(Count);
      if fControl.Canvas.TextWidth(tmpTxt+txt[Count]) > tX then
      begin
        txtStop := True;
        dec(Count);
      end
      else
      begin
        tmpTxt := tmpTxt + txt[Count];
        if txt[Count] = #32 then LastSpace := length(tmpTxt)-1;
      end;
    end;
    if (Count < length(txt)) and (LastSpace <> 0) then
    begin
      tmpTxt := copy(tmpTxt,1,LastSpace);
      Count := OCount + LastSpace + 1;
    end;
    inc(capLines);
    capWrap.Add(tmpTxt);
  end;
end;

{ This is the main window procedure }
procedure TOffice97Button.WndProc(var Message: TMessage);
begin
  case Message.Msg of
    WM_KILLFOCUS:
      MouseCapture := False;
    WM_LBUTTONDOWN, WM_LBUTTONDBLCLK:
      if not (csDesigning in ComponentState) and (not Focused) then
      begin
        { We don't allow clicks here, otherwise the control looks like it has
          been clicked twice }
        fClicksDisabled := True;
        {$IFDEF WIN32}
        Windows.SetFocus(Handle);
        {$ELSE}
        WinProcs.SetFocus(Handle);
        {$ENDIF}
        fClicksDisabled := False;
        if not Focused then Exit;
      end;
    CN_COMMAND:
      if fClicksDisabled then Exit;
  end;
  inherited WndProc(Message);
end;

constructor TOffice97Button.Create(AOwner: TComponent);
begin
  { Setup the control }
  Inherited Create(AOwner);
  CopyRightPtr := @CopyRightStr;
  fAbout := abNone;
  fActiveColor := $00808080;
  fActiveOutlineColor := clWhite;
  fActiveOutlineColor2 := $00D6E7E7;
  fInactiveColor := clSilver;
  fState := bsInactive;
  fType := bsButton;
  fToggle := false;
  fTransparent := False;
  fTransparentColor := clBlack;
  fGlyphPosition := bsLeft;
  Color := $00CCFFFF;
  fActive := TBitmap.Create;
  fActive.Handle := LoadBitmap(HInstance,'OFFICE_1');
  fDisabled := TBitmap.Create;
  fDisabled.Handle := LoadBitmap(HInstance,'OFFICE_2');
  fDownActive := TBitmap.Create;
  fDownActive.Handle := LoadBitmap(HInstance,'OFFICE_3');
  fInactive := TBitmap.Create;
  fInactive.Handle := LoadBitmap(HInstance,'OFFICE_4');
  fCustomActive := TBitmap.Create;
  fCustomDisabled := TBitmap.Create;
  fCustomDownActive := TBitmap.Create;
  fCustomInactive := TBitmap.Create;
  fDisabledWorking := TBitmap.Create;
  capWrap := TStringList.Create;
  fHoverFont := TFont.Create;
  fHoverFont.OnChange := HoverFontChanged;
  Width := 70;
  Height := 23;
  TabStop := True;
  fOffice2000Look := True;
  fShowGlyph := False;
  fBtnKey := False;
  fNoDots := False;
  fUseCustomGlyphs := False;
  fWordWrap := True;
  fAutoHeight := False;
  fAutoTransparency := True;
end;

destructor TOffice97Button.Destroy;
begin
  { Kill the control }
  fHoverFont.Free;
  capWrap.Free;
  fDisabledWorking.Free;
  fCustomInactive.Free;
  fCustomDownActive.Free;
  fCustomDisabled.Free;
  fCustomActive.Free;
  fInactive.Free;
  fDownActive.Free;
  fDisabled.Free;
  fActive.Free;
  Inherited Destroy;
end;

procedure TOffice97Button.Click;
var
  {$IFDEF OFFBTND3}
  Form: TCustomForm;
  {$ELSE}
  Form: TForm;
  {$ENDIF}
  oState: TOffBtnState;
  //Count: Integer;
begin
  oState := fState;
  if fBtnKey then
  begin
    { If a button was pressed, show the Down state }
    if fToggle then
    begin
      fDown:=not fDown;

      if fDown then
        fstate:=bsDown
      else
        ostate:=bsActive;
    end
    else fState := bsDown;
    if fState <> oState then
    begin
      Invalidate;
      Application.ProcessMessages;
    end;
  end;
  { Handle ModalResult }
  Form := GetParentForm(Self);
  { When the control is clicked, all other TOffice97Buttons should be in the
    inactive state }
  {for count := 0 to Form.ComponentCount - 1 do
    if (Form.Components[Count] is TOffice97Button) and
      (Form.Components[Count] <> Self) then
    begin
      TOffice97Button(Form.Components[Count]).fState := bsInactive;
      TOffice97Button(Form.Components[Count]).Invalidate;
    end;
  }
  if Form <> nil then Form.ModalResult := fModalResult;
  if (fBtnKey) or (fState <> oState) then
  begin
    { If a button was pressed, restore the original state }
    if not fToggle then fState := oState;
    Invalidate;
    Application.ProcessMessages;
  end;
  { Reset key pressed variable }
  fBtnKey := False;
  inherited Click;
end;

procedure TOffice97Button.CreateWnd;
begin
  inherited CreateWnd;
  fFocused := fDefault;
  fNoDots := not fDefault;
end;

{$IFDEF OFFBTND4}
procedure TOffice97Button.ActionChange(Sender: TObject; CheckDefaults: Boolean);
begin
  inherited ActionChange(Sender, CheckDefaults);
  if Sender is TCustomAction then
    with TCustomAction(Sender) do
      Self.Caption := Caption;
end;
{$ENDIF}

procedure TOffice97Button.CNCommand(var Message: TWMCommand);
begin
  if Message.NotifyCode = BN_CLICKED then Click;
end;

{ Thanks to Michel for this procedure }
procedure TOffice97Button.CNKeyDown(var Message: TWMKeyDown);
begin
  with Message do
    if ((CharCode = VK_RETURN) or (CharCode = VK_SPACE)) and fFocused then
    begin
      { Set key pressed variable }
      fBtnKey := True;
      Click;
     { If we are using a modal form, release the mouse capture }
      if fModalResult <> mrNone then
        MouseCapture := False;
      Result := 1;
    end else
      inherited;
end;

{ Thanks to Kambiz and Michel for fixing a bug in this procedure }
procedure TOffice97Button.CMDialogChar(var Message: TCMDialogChar);
begin
  with Message do
    if IsAccel(CharCode, fCaption) and CanFocus then
    begin
      { Set key pressed variable }
      fBtnKey := True;
      Click;
      { If we are using a modal form, release the mouse capture }
      if fModalResult <> mrNone then MouseCapture := False;
      Result := 1;
    end else
      inherited;
end;

procedure TOffice97Button.CMDialogKey(var Message: TCMDialogKey);
begin
  with Message do
    if (((CharCode = VK_RETURN) or (CharCode = VK_SPACE)) and fFocused)
      or ((CharCode = VK_ESCAPE) and fCancel)
      and (KeyDataToShiftState(KeyData) = []) and CanFocus then
    begin
      { Set key pressed variable }
      fBtnKey := True;
      Click;
      { If we are using a modal form, release the mouse capture }
      if fModalResult <> mrNone then MouseCapture := False;
      Result := 1;
    end
    else
    begin
      if (CharCode = VK_F4) and (KeyDataToShiftState(KeyData) = [ssAlt]) then
      begin
        if (fState=bsActive) or not fToggle then fState := bsInActive;
        MouseCapture := False;
      end;
      inherited;
    end;
end;

procedure TOffice97Button.CMEnabledChanged(var Message: TMessage);
begin
  Inherited;
  Invalidate;
end;

procedure TOffice97Button.CMFocusChanged(var Message: TMessage);
begin
  Inherited;
  Invalidate;
end;

{ This procedure picks up the focus loss }

procedure TOffice97Button.WMKillFocus(var Message: TWMKillFocus);
begin
  if (fstate=bsActive) or not Toggle then fState := bsInactive;
  fFocused := False;
  fNoDots := True;
  Invalidate;
end;

{ This procedure picks up the focus gain }

procedure TOffice97Button.WMSetFocus(var Message: TWMSetFocus);
begin
  if (fstate=bsActive) or not Toggle then fState := bsInactive;
  fFocused := True;
  fNoDots := False;
  Invalidate;
end;

{ Start of component configuration routines

  Thanks to Kambiz for adding auto height adjustment to them }

procedure TOffice97Button.SetCaption(Val: TCaption);
begin
  if fCaption <> Val then
  begin
    fCaption := Val;
    if fAutoHeight then AdjustHeight;
    Invalidate;
  end;
end;

{ Thanks to Kambiz for this procedure }
procedure TOffice97Button.SetAutoHeight(Val: Boolean);
begin
  if fAutoHeight <> Val then
  begin
    fAutoHeight := Val;
    if fAutoHeight then AdjustHeight;
    Invalidate;
  end;
end;

procedure TOffice97Button.SetDown(Val: Boolean);
var
  oState: TOffBtnState;
begin
  Inherited;
  oState := fState;
  fDown:=Val;

  if (fDown) then fState:=bsDown
  else fState:=bsInactive;

  if oState <> fState then Invalidate;
end;

procedure TOffice97Button.SetAutoTransparency(Val: Boolean);
begin
  if fAutoTransparency <> Val then
  begin
    fAutoTransparency := Val;
    Invalidate;
  end;
end;

procedure TOffice97Button.SetDefault(Value: Boolean);
begin
  fDefault := Value;
  with GetParentForm(Self) do
    Perform(cm_FocusChanged, 0, Longint(ActiveControl));
end;

procedure TOffice97Button.SetActiveColor(Val: TColor);
begin
  if fActiveColor <> Val then
  begin
    fActiveColor := Val;
    Invalidate;
  end;
end;

procedure TOffice97Button.SetActiveOutlineColor(Val: TColor);
begin
  if fActiveOutlineColor <> Val then
  begin
    fActiveOutlineColor := Val;
    Invalidate;
  end;
end;

procedure TOffice97Button.SetActiveOutlineColor2(Val: TColor);
begin
  if fActiveOutlineColor2 <> Val then
  begin
    fActiveOutlineColor2 := Val;
    Invalidate;
  end;
end;

procedure TOffice97Button.SetControlType(Val: TOffBtnType);
begin
  { Load the default glyphs for the new Control type }
  if fType <> Val then
  begin
    fType := Val;
    case fType of
      bsButton:
        begin
          fActive.Handle := LoadBitmap(HInstance,'OFFICE_1');
          fDisabled.Handle := LoadBitmap(HInstance,'OFFICE_2');
          fDownActive.Handle := LoadBitmap(HInstance,'OFFICE_3');
          fInactive.Handle := LoadBitmap(HInstance,'OFFICE_4');
        end;
      bsRadioButton:
        begin
          if fOffice2000Look then
          begin
            fActive.Handle := LoadBitmap(HInstance,'OFFICE_17');
            fDisabled.Handle := LoadBitmap(HInstance,'OFFICE_18');
            fDownActive.Handle := LoadBitmap(HInstance,'OFFICE_19');
            fInactive.Handle := LoadBitmap(HInstance,'OFFICE_20');
          end
          else
          begin
            fActive.Handle := LoadBitmap(HInstance,'OFFICE_5');
            fDisabled.Handle := LoadBitmap(HInstance,'OFFICE_6');
            fDownActive.Handle := LoadBitmap(HInstance,'OFFICE_7');
            fInactive.Handle := LoadBitmap(HInstance,'OFFICE_8');
          end;
        end;
      bsUpButton:
        begin
          fActive.Handle := LoadBitmap(HInstance,'OFFICE_9');
          fDisabled.Handle := LoadBitmap(HInstance,'OFFICE_10');
          fDownActive.Handle := LoadBitmap(HInstance,'OFFICE_11');
          fInactive.Handle := LoadBitmap(HInstance,'OFFICE_12');
        end;
      bsDownButton:
        begin
          fActive.Handle := LoadBitmap(HInstance,'OFFICE_13');
          fDisabled.Handle := LoadBitmap(HInstance,'OFFICE_14');
          fDownActive.Handle := LoadBitmap(HInstance,'OFFICE_15');
          fInactive.Handle := LoadBitmap(HInstance,'OFFICE_16');
        end;
      bsHintButton:
        begin
          fActive.Handle := LoadBitmap(HInstance,'OFFICE_21');
          fDisabled.Handle := LoadBitmap(HInstance,'OFFICE_22');
          fDownActive.Handle := LoadBitmap(HInstance,'OFFICE_23');
          fInactive.Handle := LoadBitmap(HInstance,'OFFICE_24');
        end;
    end;
    if (fType <> bsButton) and (fGlyphPosition in [bsLeft,bsRight]) then
      fGlyphPosition := bsTop;
    if fAutoHeight then AdjustHeight;
    ShowGlyph := not (Office2000Look and (fType = bsButton));
    Invalidate;
  end;
end;

procedure TOffice97Button.SetCustomActiveGlyph(Val: TBitmap);
begin
  fCustomActive.Assign(Val);
  if fAutoHeight then AdjustHeight;
  Invalidate;
end;

procedure TOffice97Button.SetCustomDisabledGlyph(Val: TBitmap);
begin
  fCustomDisabled.Assign(Val);
  if fAutoHeight then AdjustHeight;
  Invalidate;
end;

procedure TOffice97Button.SetCustomDownActiveGlyph(Val: TBitmap);
begin
  fCustomDownActive.Assign(Val);
  if fAutoHeight then AdjustHeight;
  Invalidate;
end;

procedure TOffice97Button.SetCustomInactiveGlyph(Val: TBitmap);
begin
  fCustomInactive.Assign(Val);
  if fAutoHeight then AdjustHeight;
  Invalidate;
end;

procedure TOffice97Button.SetGlyphPosition(Val: TGlyphPosition);
begin
  if fGlyphPosition <> Val then
  begin
    fGlyphPosition := Val;
    if (fType <> bsButton) and (fGlyphPosition in [bsLeft,bsRight]) then
      fGlyphPosition := bsTop;
    if fAutoHeight then AdjustHeight;
    Invalidate;
  end;
end;

procedure TOffice97Button.SetInactiveColor(Val: TColor);
begin
  if fInactiveColor <> Val then
  begin
    fInactiveColor := Val;
    Invalidate;
  end;
end;

procedure TOffice97Button.SetOffice2000Look(Val: Boolean);
begin
  if fOffice2000Look <> Val then
  begin
    fOffice2000Look := Val;
    if fOffice2000Look = True then
    begin
      { Setup control for Office 2000 look }
      fActiveColor := $00808080;
      fActiveOutlineColor := clWhite;
      fActiveOutlineColor2 := $00D6E7E7;
      fInactiveColor := clSilver;
      Color := $00CCFFFF;
      { Normal buttons don't have glyphs }
      if fType = bsButton then fShowGlyph := False;
      { Update radiobutton glyphs }
      if fType = bsRadioButton then
      begin
        fActive.Handle := LoadBitmap(HInstance,'OFFICE_17');
        fDisabled.Handle := LoadBitmap(HInstance,'OFFICE_18');
        fDownActive.Handle := LoadBitmap(HInstance,'OFFICE_19');
        fInactive.Handle := LoadBitmap(HInstance,'OFFICE_20');
      end;
    end
    else
    begin
      { Setup the control for Office 97 look }
      fActiveColor := clGray;
      fActiveOutlineColor := clWhite;
      fInactiveColor := clSilver;
      fState := bsInactive;
      Color := $00E1FFFF;
      fShowGlyph := True;
      { Update radiobutton glyphs }
      if fType = bsRadioButton then
      begin
        fActive.Handle := LoadBitmap(HInstance,'OFFICE_5');
        fDisabled.Handle := LoadBitmap(HInstance,'OFFICE_6');
        fDownActive.Handle := LoadBitmap(HInstance,'OFFICE_7');
        fInactive.Handle := LoadBitmap(HInstance,'OFFICE_8');
      end
    end;
    Invalidate;
  end;
end;

procedure TOffice97Button.SetShowGlyph(Val: Boolean);
begin
  if fShowGlyph <> Val then
  begin
    fShowGlyph := Val;
    if fAutoHeight then AdjustHeight;
    Invalidate;
  end;
end;

procedure TOffice97Button.SetTransparent(Val: Boolean);
begin
  if fTransparent <> Val then
  begin
    fTransparent := Val;
    Invalidate;
  end;
end;

procedure TOffice97Button.SetTransparentColor(Val: TColor);
begin
  if fTransparentColor <> Val then
  begin
    fTransparentColor := Val;
    Invalidate;
  end;
end;

procedure TOffice97Button.SetUseCustomGlyphs(Val: Boolean);
begin
  if fUseCustomGlyphs <> Val then
  begin
    fUseCustomGlyphs := Val;
    if fAutoHeight then AdjustHeight;
    Invalidate;
  end;
end;

procedure TOffice97Button.SetWordWrap(Val: Boolean);
begin
  if fWordWrap <> Val then
  begin
    fWordWrap := Val;
    if fAutoHeight then AdjustHeight;
    Invalidate;
  end;
end;

procedure TOffice97Button.SetHoverFont(Val: TFont);
begin
  fHoverFont.Assign(Val);
end;

procedure TOffice97Button.HoverFontChanged(Sender: TObject);
begin
  Invalidate;
end;

procedure TOffice97Button.ShowAbout(Val: TOffBtnAbout);
begin
  if fAbout <> Val then
  begin
    if Val = abNone then fAbout := Val else
    begin
      fAbout := abNone;
      MessageDlg(StrPas(CopyRightStr), mtInformation, [mbOk], 0);
    end;
    Invalidate;
  end;
end;

{ End of component configuration routines }

{ Thanks to Kambiz for adding auto height adjustment to this procedure }
function TOffice97Button.CurrentGlyph: TBitmap;
begin
  { Default to inactive glyph (Or custom inactive glyph, if set)
    If nessacary, work out the glyph (Or custom glyph, if set)
    to display }
  case fState of
    bsActive:
    begin
      if not fUseCustomGlyphs then Result := fActive
        else
      begin
        if fCustomActive.Empty then Result := fCustomInactive
          else Result := fCustomActive;
      end;
    end;
    bsDown:
    begin
      if not fUseCustomGlyphs then Result := fDownActive
        else
      begin
        if fCustomDownActive.Empty then Result := fCustomInactive
          else Result := fCustomDownActive;
      end;
    end
    else
    begin
      if not fUseCustomGlyphs then Result := fInactive
        else Result := fCustomInactive;
    end;
  end;
  if not Enabled then
  begin
    if not fUseCustomGlyphs then Result := fDisabled
      else
    begin
      if fCustomDisabled.Empty then
      begin
        CreateDisabledBitmap(fCustomInactive,clBlack);
        Result := fDisabledWorking;
      end
      else
        Result := fCustomDisabled;
    end;
  end;
end;

{ This procedure draws an Office 97 style frame }
procedure TOffice97Button.DrawFrame;
var
  rClient: TRect;
begin
  rClient := ClientRect;
  with fControl.Canvas do
  begin
    with rClient do
    begin
      Pen.Color := fInactiveColor;
      Pen.Style := psSolid;
      { Draw the appropriate state frame }
      case fState of
        bsActive:
          begin
            PolyLine([Point(Right-8,1),Point(8,1),Point(7,2),
              Point(6,2),Point(2,6),Point(2,7),Point(1,8),
              Point(1,Bottom-8),Point(2,Bottom-7),
              Point(2,Bottom-6),Point(5,Bottom-3)]);
            Pixels[5,Bottom-3] := fInactiveColor;
            Pixels[7,Bottom-2] := fInactiveColor;
            PolyLine([Point(5,Bottom-5),Point(6,Bottom-4),
              Point(7,Bottom-4),Point(8,Bottom-3),
              Point(Right-8,Bottom-3),Point(Right-3,Bottom-8),
              Point(Right-3,8),Point(Right-2,7)]);
            Pixels[Right-2,7] := fInactiveColor;
            Pixels[Right-7,Bottom-2] := fInactiveColor;
            PolyLine([Point(Right-5,Bottom-3),
              Point(Right-3,Bottom-5)]);
            Pixels[Right-3,Bottom-5] := fInactiveColor;
            PolyLine([Point(Right-6,Bottom-4),
              Point(Right-4,Bottom-6)]);
            Pixels[Right-4,Bottom-6] := fInactiveColor;
            Pixels[Right-2,Bottom-7] := fInactiveColor;
            Pixels[Right-2,7] := fInactiveColor;
            PolyLine([Point(Right-4,6),Point(Right-5,5)]);
            Pixels[Right-5,5] := fInactiveColor;
            PolyLine([Point(Right-3,5),Point(Right-6,2)]);
            Pixels[Right-6,2] := fInactiveColor;
            Pen.Color := fActiveColor;
            PolyLine([Point(8,2),Point(7,3),Point(6,3),
              Point(3,6),Point(3,7),Point(2,8),
              Point(2,Bottom-8),Point(3,Bottom-7),
              Point(3,Bottom-6),Point(6,Bottom-3),
              Point(7,Bottom-3),Point(8,Bottom-2),
              Point(Right-8,Bottom-2),Point(Right-7,Bottom-3),
              Point(Right-6,Bottom-3),Point(Right-3,Bottom-6),
              Point(Right-3,Bottom-7),Point(Right-2,Bottom-8),
              Point(Right-2,8),Point(Right-3,7),
              Point(Right-3,6),Point(Right-7,2),
              Point(8,2)]);
            Pixels[8,2] := fActiveColor;
            Pen.Color := fActiveOutlineColor;
            PolyLine([Point(Right-7,3),Point(8,3),
              Point(3,8),Point(3,Bottom-8)]);
            Pixels[3,Bottom-8] := fActiveOutlineColor;
            PolyLine([Point(Right-6,5),Point(Right-6,4),
              Point(8,4),Point(4,8),Point(4,Bottom-6),
              Point(5,Bottom-6)]);
            Pixels[5,Bottom-6] := fActiveOutlineColor;
            PolyLine([Point(6,4),Point(4,6)]);
            Pixels[4,6] := fActiveOutlineColor;
          end;
        bsDown:
          begin
            PolyLine([Point(Right-8,1),Point(8,1),
              Point(7,2),Point(6,2),Point(2,6),Point(2,7),
              Point(1,8),Point(1,Bottom-8),Point(2,Bottom-7),
              Point(2,Bottom-6),Point(4,Bottom-4),
              Point(5,Bottom-4),Point(6,Bottom-3),
              Point(7,Bottom-3),Point(8,Bottom-2),
              Point(Right-8,Bottom-2),Point(Right-7,Bottom-3),
              Point(Right-6,Bottom-3),Point(Right-3,Bottom-6),
              Point(Right-3,Bottom-7),Point(Right-2,Bottom-8),
              Point(Right-2,8),Point(Right-3,7),
              Point(Right-3,6)]);
            Pixels[Right-3,6] := fInactiveColor;
            Pixels[8,3] := fInactiveColor;
            PolyLine([Point(6,4),Point(4,6)]);
            Pixels[4,6] := fInactiveColor;
            PolyLine([Point(3,8),Point(3,Bottom-8)]);
            Pixels[3,Bottom-8] := fInactiveColor;
            Pixels[4,Bottom-6] := fInactiveColor;
            PolyLine([Point(Right-4,4),Point(Right-6,2)]);
            Pixels[Right-6,2] := fInactiveColor;
            Pixels[Right-7,3] := fInactiveColor;
            Pen.Color := fActiveColor;
            PolyLine([Point(Right-4,5),Point(Right-7,2),
              Point(8,2),Point(7,3),Point(6,3),Point(3,6),
              Point(3,7),Point(2,8),Point(2,Bottom-8),
              Point(3,Bottom-7),Point(3,Bottom-6),
              Point(4,Bottom-5)]);
            Pixels[4,Bottom-5] := fActiveColor;
            Pen.Color := fActiveOutlineColor;
            PolyLine([Point(7,5),Point(5,7),
              Point(5,Bottom-5)]);
            Pixels[5,Bottom-5] := fActiveOutlineColor;
            PolyLine([Point(Right-8,3),Point(9,3)]);
            Pixels[9,3] := fActiveOutlineColor;
            PolyLine([Point(5,Bottom-3),Point(6,Bottom-2),
              Point(7,Bottom-2),Point(7,Bottom-1),
              Point(Right-6,Bottom-1),Point(Right-1,Bottom-6),
              Point(Right-1,8),Point(Right-2,7),
              Point(Right-2,6),Point(Right-3,5),
              Point(Right-4,6),Point(Right-5,5),
              Point(Right-6,5),Point(Right-6,4),Point(7,4),
              Point(4,7),Point(4,Bottom-7)]);
            Pixels[4,Bottom-7] := fActiveOutlineColor;
            PolyLine([Point(Right-4,Bottom-2),
              Point(Right-1,Bottom-5)]);
            Pixels[Right-1,Bottom-5] := fActiveOutlineColor;
            PolyLine([Point(Right-7,Bottom-2),
              Point(Right-6,Bottom-2),Point(Right-2,Bottom-6),
              Point(Right-2,Bottom-7)]);
            Pixels[Right-2,Bottom-7] := fActiveOutlineColor;
          end;
        bsDownAndOut,bsInactive:
          begin
            PolyLine([Point(8,2),Point(7,3),Point(6,3),
              Point(3,6),Point(3,7),Point(2,8),
              Point(2,Bottom-8),Point(3,Bottom-7),
              Point(3,Bottom-6),Point(6,Bottom-3),
              Point(7,Bottom-3),Point(8,Bottom-2),
              Point(Right-8,Bottom-2),Point(Right-7,Bottom-3),
              Point(Right-6,Bottom-3),Point(Right-3,Bottom-6),
              Point(Right-3,Bottom-7),Point(Right-2,Bottom-8),
              Point(Right-2,8),Point(Right-3,7),
              Point(Right-3,6),Point(Right-6,3),
              Point(Right-7,3),Point(Right-8,2),Point(8,2)]);
            Pixels[8,2] := fInactiveColor;
          end;
      end;
    end;
  end;
end;

{ This procedure draws an Office 2000 style frame }
procedure TOffice97Button.DrawOffice2000Frame;
var
  rClient: TRect;
begin
  rClient := ClientRect;
  with fControl.Canvas do
  begin
    with rClient do
    begin
      Pen.Color := fInactiveColor;
      Pen.Style := psSolid;
      { Draw the appropriate state frame }
      case fState of
        bsActive:
          begin
            Pixels[Right-2,3] := fInactiveColor;
            Pixels[2,Bottom-3] := fInactiveColor;
            Pen.Color := fActiveColor;
            PolyLine([Point(Right-2,4),Point(Right-2,Bottom-4),
              Point(Right-4,Bottom-2),Point(3,Bottom-2)]);
            Pixels[3,Bottom-2] := fActiveColor;
            Pen.Color := fActiveOutlineColor;
            PolyLine([Point(Right-3,2),Point(Right-4,1),
              Point(3,1),Point(1,3),Point(1,Bottom-4)]);
            Pixels[1,Bottom-4] := fActiveOutlineColor;
            Pen.Color := fActiveOutlineColor2;
            PolyLine([Point(3,0),Point(0,3),
              Point(0,Bottom-4),Point(3,Bottom-1),
              Point(Right-4,Bottom-1),Point(Right-1,Bottom-4),
              Point(Right-1,3),Point(Right-4,0),Point(3,0)]);
          end;
        bsDown:
          begin
            Pixels[Right-3,2] := fInactiveColor;
            Pixels[1,Bottom-4] := fInactiveColor;
            Pen.Color := fActiveColor;
            PolyLine([Point(Right-4,1),Point(3,1),
              Point(1,3),Point(1,Bottom-5)]);
            Pixels[1,Bottom-5] := fActiveColor;
            Pen.Color := fActiveOutlineColor;
            PolyLine([Point(Right-2,2),Point(Right-1,3),
              Point(Right-1,Bottom-4),Point(Right-4,Bottom-1),
              Point(3,Bottom-1),Point(1,Bottom-3)]);
            Pixels[1,Bottom-3] := fActiveOutlineColor;
            Pen.Color := fActiveOutlineColor2;
            PolyLine([Point(Right-3,1),Point(Right-4,0),
              Point(3,0),Point(0,3),Point(0,Bottom-4)]);
            Pixels[0,Bottom-4] := fActiveOutlineColor2;
            PolyLine([Point(Right-2,3),Point(Right-2,Bottom-4),
              Point(Right-4,Bottom-2),Point(3,Bottom-2),
              Point(2,Bottom-3)]);
            Pixels[2,Bottom-3] := fActiveOutlineColor2;
          end;
        bsDownAndOut,bsInactive:
          begin
            PolyLine([Point(3,1),Point(1,3),
              Point(1,Bottom-4),Point(3,Bottom-2),
              Point(Right-4,Bottom-2),Point(Right-2,Bottom-4),
              Point(Right-2,3),Point(Right-4,1),Point(3,1)]);
          end;
      end;
    end;
  end;
end;

{ Thanks to Kambiz for adding bi-directional support to this procedure }
procedure TOffice97Button.Layout(var txtRect, bitRect: TRect);
var
  dBit, hBit, vBit, hTxt, vTxt: Integer;
  GlyphPos: TGlyphPosition;
begin
  { Work out text canvas height and width }
  hTxt := txtRect.Right - txtRect.Left;
  vTxt := txtRect.Bottom - txtRect.Top;
  if fShowGlyph then
  begin
    GlyphPos := fGlyphPosition;
    { Work out glyph canvas height and width }
    hBit := bitRect.Right - bitRect.Left;
    vBit := bitRect.Bottom - bitRect.Top;
    { Position glyph canvas and text canvas }
    if fType = bsButton then
    begin
      {$IFDEF OFFBTND4}
      if UseRightToLeftAlignment then
      begin
        if GlyphPos = bsLeft then
          GlyphPos := bsRight
        else if GlyphPos = bsRight then
          GlyphPos := bsLeft
      end;
      {$ENDIF}
      case GlyphPos of
        bsTop, bsBottom:
        begin
          { bsTop positioning }
          bitRect.Left := ((Width - hBit - 1) div 2) + 1;
          txtRect.Left := ((Width - hTxt - 1) div 2) + 1;
          bitRect.Top := 6;
          txtRect.Top := ((Height - (vBit + vTxt) - 1) div 2) + vBit + 1;
          if GlyphPos = bsBottom then
          begin
            { Mirror top coordinates for bsBottom }
            bitRect.Top := Height - (bitRect.Top + vBit) - 1;
            txtRect.Top := Height - (txtRect.Top + vTxt) - 1;
          end;
        end;
        bsLeft, bsRight:
        begin
          { bsLeft positioning }
          bitRect.Top := ((Height - vBit - 1) div 2) + 1;
          txtRect.Top := ((Height - vTxt - 1) div 2) + 1;
          bitRect.Left := 6;
          txtRect.Left := ((Width - (hBit + hTxt) - 1) div 2) + hBit + 1;
          if GlyphPos = bsRight then
          begin
            { Mirror left coordinates for bsRight }
            bitRect.Left := Width - (bitRect.Left + hBit) - 1;
            txtRect.Left := Width - (txtRect.Left + hTxt) - 1;
          end;
        end;
      end;
    end
    else
    begin
      { bsTop positioning }
      dBit := fControl.Canvas.TextHeight(fCaption) - vBit;
      if dBit < 2 then
        bitRect.Top := 2
      else
        bitRect.Top := 2 + (dBit div 2);
      txtRect.Top := 2;
      bitRect.Left := 2;
      txtRect.Left := hBit + 7;
      {$IFDEF OFFBTND4}
      if UseRightToLeftAlignment then
      begin
        bitRect.Left := (Width - hBit) - bitRect.Left;
        txtRect.Left := (Width - hTxt) - txtRect.Left;
      end;
      {$ENDIF}
      if GlyphPos = bsBottom then
      begin
        { Mirror top coordinates for bsBottom }
        bitRect.Top := Height - (bitRect.Top + vBit) - 1;
        txtRect.Top := Height - (txtRect.Top + vTxt) - 1;
      end;
    end;
    { Set the glyph canvas height and width }
    bitRect.Right := bitRect.Left + hBit;
    bitRect.Bottom := bitRect.Top + vBit;
  end
  else
  begin
    { Center, or left justify, the text canvas }
    if fType = bsButton then
    begin
      txtRect.Top := ((Height - vTxt - 1) div 2) + 1;
      txtRect.Left := ((Width - hTxt - 1) div 2) + 1;
    end
    else
    begin
      txtRect.Top := 2;
      txtRect.Left := 2;
      {$IFDEF OFFBTND4}
      if UseRightToLeftAlignment then
        txtRect.Left := (Width - hTxt) - txtRect.Left;
      {$ENDIF}
    end;
  end;
  { Set the text canvas height and width }
  txtRect.Right := txtRect.Left + hTxt;
  txtRect.Bottom := txtRect.Top + vTxt;
  { Draw the focus using the appropriate style }
  with fControl.Canvas do
  begin
    if (fCaption <> '') and ((csDesigning in ComponentState)
      and Enabled) or (not(csDesigning in ComponentState)
        and (not fNoDots) and (Focused or (fFocused and
          not(Screen.ActiveControl is TOffice97Button)))) then
    begin
      if fType = bsButton then
        {$IFDEF WIN32}
        Windows.DrawFocusRect(Handle,Rect(txtRect.Left,txtRect.Top,txtRect.Right+1,txtRect.Bottom+1))
        {$ELSE}
        WinProcs.DrawFocusRect(Handle,Rect(txtRect.Left,txtRect.Top,txtRect.Right+1,txtRect.Bottom+1))
        {$ENDIF}
      else
        DrawOfficeFocusRect(txtRect,capWrap);
    end;
  end;
  { If control down, and control type is button, draw
    text and glyph down and to the right }
  if (fState = bsDown) and (fType = bsButton) then
  begin
    if fShowGlyph then OffsetRect(bitRect, 1, 1);
    OffsetRect(txtRect, 1, 1);
  end;
end;

procedure TOffice97Button.CalculateTxt(var txtRect: TRect;Glyph: TBitmap);
begin
  { If text is to be wordwrapped, the rectangle size must be
    based on the control size and glyph position - TextWidth and
    TextHeight give the size but they assume that the text won't
    be wordwrapped }
  if fType = bsButton then
  begin
    if fGlyphPosition in [bsLeft,bsRight] then
    begin
      tX := width - glyph.width - 18;
      if not fShowGlyph then inc(tX,glyph.width + 5);
    end
    else
      tX := width - 13;
  end
  else
  begin
    tX := width - glyph.width - 10;
    if not fShowGlyph then inc(tX,glyph.width + 5);
  end;
  with fControl.Canvas do
  begin
    { Wordwrap text and store the result in a string list }
    GetWrapText(fCaption,tX);
    if TextWidth(fCaption) > tX then
      txtRect := Rect(0, 0, tX, capLines*TextHeight('0'))
    else
    begin
      tX := TextWidth(fCaption);
      txtRect := Rect(0, 0, TextWidth(fCaption), TextHeight(fCaption));
    end;
  end;
end;

procedure TOffice97Button.WMEraseBkgnd(var Message: TWMEraseBkgnd);
begin
  Message.Result := 1;
end;

{ Thanks to Kambiz for this procedure }
procedure TOffice97Button.AdjustHeight;
var
  txtRect: TRect;
  hBit, hTxt: Integer;
begin
  fControl := TBitmap.Create;
  fControl.Width := Width;
  fControl.Height := Height;
  fControl.Canvas.Font := Font;
  if not fWordWrap then
    hTxt := fControl.Canvas.TextHeight(fCaption)
  else
  begin
    CalculateTxt(txtRect, CurrentGlyph);
    hTxt := txtRect.Bottom;
  end;
  if fShowGlyph then
  begin
    hBit := CurrentGlyph.Height;
    if (fType = bsButton) and (fGlyphPosition in [bsTop,bsBottom]) then
      Inc(hTxt, hBit + 4)
    else if hBit > htxt then
      hTxt := hBit;
  end;
  if fType = bsButton then
    Height := hTxt + 10
  else
    Height := hTxt + 5;
  fControl.Free;
end;
procedure TOffice97Button.PerformEraseBackground(Control: TControl; DC: HDC);

// Repainting the background of a control using theme services relies on the ability of the parent to handle
// WM_PRINT messages. Usually the default behavior of a window is enough to make this possible. However
// double buffered and non-windowed controls are quite different and need so special handling.
// This method uses the WM_ERASEBKGND message to achieve the same effect.

var
  LastOrigin: TPoint;

begin
  GetWindowOrgEx(DC, LastOrigin);
  SetWindowOrgEx(DC, LastOrigin.X + Control.Left, LastOrigin.Y + Control.Top, nil);
  Control.Parent.Perform(WM_ERASEBKGND, Integer(DC), Integer(DC));
  SetWindowOrgEx(DC, LastOrigin.X, LastOrigin.Y, nil);
end;

{ Thanks to Kambiz for adding bi-directional support to this procedure }
procedure TOffice97Button.Paint;
var
  Glyph: TBitmap;
  tmpRect,txtRect, bitRect, glyphRect: TRect;
  TempCap: array[0..255] of char;
  Count: Integer;
  DrawFlags: LongInt;
begin
  PerformEraseBackground(Self, Canvas.Handle);
  { Setup the offscreen bitmap }
  fControl := TBitmap.Create;
  fControl.Width := Width;
  fControl.Height := Height;
  with fControl.Canvas do
  begin
    { Fill control background }
    if fTransparent then
      fControl.Canvas.CopyRect(Rect(0,0,Width,Height), Canvas, Rect(0,0,Width,Height))
      //CopyParentImage(Self, fControl.Canvas)
    else
    begin
      Brush.Color := Color;
      Brush.Style := bsSolid;
      FillRect(ClientRect);
    end;
    { Figure out size of text and display bitmaps }
    if not (Enabled and (fState in [bsActive, bsDown])) then
    begin
      Font := Self.Font;
      if not enabled then Font.Color := fInactiveColor;
    end
    else
      Font := fHoverFont;
    if fDefault then Font.Style := Font.Style + [fsBold];
    Glyph := CurrentGlyph;
    bitRect := Rect(0, 0, Glyph.Width, Glyph.Height);
    if not fWordWrap then
      txtRect := Rect(0, 0, TextWidth(fCaption), TextHeight(fCaption))
    else
      CalculateTxt(txtRect,Glyph);
    glyphRect := bitRect;
    { Calculate position of text and bitmap and draw focus }
    Layout(txtRect,bitRect);
    { Draw the caption }
    Brush.Style := bsClear;
    SetBkMode(Handle, {$IFDEF WIN32}Windows{$ELSE}WinTypes{$ENDIF}.TRANSPARENT);
    StrPCopy(TempCap, fCaption);
    if (not fWordWrap) or (capLines = 1) then
    begin
      { A single line caption }
      if fType = bsButton then
        DrawFlags := DT_CENTER
      else
        DrawFlags := DT_WORDBREAK;
      {$IFDEF OFFBTND4}
      DrawFlags := DrawTextBiDiModeFlags(DrawFlags);
      {$ENDIF}
      DrawText(Handle, TempCap, StrLen(TempCap), txtRect, DrawFlags);
    end
    else
      { A multiple line caption }
      for Count := 0 to capWrap.Count-1 do
      begin
        StrPCopy(TempCap, capWrap.Strings[Count]);
        tmpRect := Rect(0, 0, tX, TextHeight('0'));
        tmpRect.Left := txtRect.Left;
        tmpRect.Top := txtRect.Top+(Count*TextHeight('0'));
        tmpRect.Right := tmpRect.Left + tX;
        tmpRect.Bottom := tmpRect.Top + TextHeight('0');
        if fType = bsButton then
          DrawFlags := DT_CENTER
        else
          DrawFlags := 0;
        {$IFDEF OFFBTND4}
        DrawFlags := DrawTextBiDiModeFlags(DrawFlags);
        {$ENDIF}
        DrawText(Handle, TempCap, StrLen(TempCap), tmpRect, DrawFlags);
      end;
    { Draw the glyph, if required }
    if fShowGlyph then
    begin
      if fAutoTransparency then
        DrawTransparentBitmap(fControl.Canvas, bitRect.Left, bitRect.Top,
          Glyph, Glyph.Canvas.Pixels[0, Glyph.Height-1])
      else
        DrawTransparentBitmap(fControl.Canvas, bitRect.Left, bitRect.Top,
          Glyph, fTransparentColor);
    end;
    { Finally, draw control frame if it is a button type }
    if fType = bsButton then
    begin
      if (not fTransparent) or (fState in [bsActive, bsDown]) then
      begin
        { Work out which frame style to use }
        if fOffice2000Look then DrawOffice2000Frame else
          DrawFrame;
      end;
    end;
  end;
  { Now copy the bitmap to the screen and free it }
  Canvas.CopyRect(Rect(0,0,Width,Height), fControl.Canvas, Rect(0,0,Width,Height));
  fControl.Free;
end;

{ Start of mouse routines

  FindDragTarget is much better than using PtInRect as it takes into
  account the Z order of controls }

procedure TOffice97Button.WMLButtonDown(var Message: TWMLButtonDown);
var
  InControl: Boolean;
  oState: TOffBtnState;
  Temp: TPoint;
begin
  Inherited;
  oState := fState;
  Temp := ClientToScreen(Point(Message.XPos,Message.YPos));
  InControl := FindDragTarget(Temp, True) = Self;
  if InControl then
  begin
    MouseCapture := True;
    fState := bsDown;
  end;
  if oState <> fState then Invalidate;
end;

procedure TOffice97Button.WMMouseMove(var Message: TWMMouseMove);
var
  InControl: Boolean;
  oState: TOffBtnState;
  Temp: TPoint;
begin
  Inherited;
  oState := fState;
  Temp := ClientToScreen(Point(Message.XPos,Message.YPos));
  InControl := FindDragTarget(Temp, True) = Self;

  if Toggle and not InControl then
  begin
    case fState of
    bsInactive:
      if (fDown) then fState:=bsDown
      else fState:=bsInActive;

    bsActive:
      if (fDown) then fState:=bsDown
      else fState:=bsInactive;

    bsDown:
        fState:=bsDown;
    end;
    MouseCapture:=false
  end
  else
    case fState of
    bsInActive:  if InControl then
                 begin
                   fState := bsActive;
                   if Assigned(fMouseEnter) then fMouseEnter(Self);
                   MouseCapture := True;
                 end;
    bsActive:    if not InControl then
                 begin
                   fState := bsInActive;
                   if Assigned(fMouseExit) then fMouseExit(Self);
                   MouseCapture := False;
                 end;
    end;
  if oState <> fState then Invalidate;
end;

procedure TOffice97Button.WMLButtonUp(var Message: TWMLButtonUp);
var
  InControl: Boolean;
  oState: TOffBtnState;
  Temp: TPoint;
begin
  Inherited;
  oState := fState;
  Temp := ClientToScreen(Point(Message.XPos,Message.YPos));
  InControl := FindDragTarget(Temp, True) = Self;
  { If we are using a modal form, we release the mouse capture }
  if (InControl) and (fModalResult = mrNone) then
  begin
    if (fToggle) then
    begin
      if fDown then
      begin
        fstate:=bsDown
      end
      else
      begin
        fstate:=bsActive
      end
    end
    else  fState := bsActive;
    MouseCapture := True;
  end
  else
  begin
    if (fState=bsActive) or not fToggle then fState := bsInactive;
    MouseCapture := False;
  end;
  if oState <> fState then Invalidate;
end;

{ This procedure ensures that the control state is correct when
  the popup menu is displayed }
procedure TOffice97Button.WMRButtonDown(var Message: TWMRButtonDown);
var
  InControl: Boolean;
  oState: TOffBtnState;
  Temp: TPoint;
begin
  Inherited;
  oState := fState;
  Temp := ClientToScreen(Point(Message.XPos,Message.YPos));
  InControl := FindDragTarget(Temp, True) = Self;
  if (InControl) and (PopupMenu <> nil) then
  begin
    if (fState=bsActive) or not fToggle then fState := bsInactive;
    MouseCapture := False;
  end;
  if oState <> fState then Invalidate;
end;

{ End of mouse routines }

procedure Register;
begin
  RegisterComponents('Standard', [TOffice97Button]);
end;

end.

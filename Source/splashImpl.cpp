                 spl->SetProgress(33,"");
                 spl->IncProgress("Main Timer"); Application->CreateForm(__classid(TForm1), &Form1);

                 if (ShowSplash)
                 {
                        spl->Show();
                        Application->ProcessMessages();
                 }

                 spl->IncProgress("About Box"); Application->CreateForm(__classid(TAbout), &About);
                 spl->IncProgress("Keyboard"); Application->CreateForm(__classid(TKeyboard), &Keyboard);
                 spl->IncProgress("Keyboard"); Application->CreateForm(__classid(TKeyboardFunctions), &KeyboardFunctions);
                 spl->IncProgress("Keyboard"); Application->CreateForm(__classid(TKeypad128), &Keypad128);
                 spl->IncProgress("Keyboard Helper"); Application->CreateForm(__classid(TKb), &Kb);
                 spl->IncProgress("Speed"); Application->CreateForm(__classid(TSpeed), &Speed);
                 spl->IncProgress("Memory Manager"); Application->CreateForm(__classid(TMemSave), &MemSave);
                 spl->IncProgress("Z80 Debugger"); Application->CreateForm(__classid(TDbg), &Dbg);
                 spl->IncProgress("Z80 Debugger"); Application->CreateForm(__classid(TMemoryWindow), &MemoryWindow);
                 spl->IncProgress("Z80 Debugger"); Application->CreateForm(__classid(TEditValue), &EditValue);
                 spl->IncProgress("Z80 Debugger"); Application->CreateForm(__classid(TSetBreakpoint), &SetBreakpoint);
                 spl->IncProgress("Z80 Debugger"); Application->CreateForm(__classid(TSearchSequence), &SearchSequence);
                 spl->IncProgress("Z80 Debugger"); Application->CreateForm(__classid(THistoryBox), &HistoryBox);
                 spl->IncProgress("Z80 Debugger"); Application->CreateForm(__classid(TSymbolBrowser), &SymbolBrowser);
                 spl->IncProgress("Z80 Debugger"); Application->CreateForm(__classid(TLiveMemoryWindow), &LiveMemoryWindow);
                 spl->IncProgress("Z80 Debugger"); Application->CreateForm(__classid(TProfiler), &Profiler);
                 spl->IncProgress("Z80 Debugger"); Application->CreateForm(__classid(TProfilePlot), &ProfilePlot);
                 spl->IncProgress("TZX Manager"); Application->CreateForm(__classid(TTZX), &TZX);
                 spl->IncProgress("TZX Manager"); Application->CreateForm(__classid(TEditPauseForm), &EditPauseForm);
                 spl->IncProgress("TZX Manager"); Application->CreateForm(__classid(TEditArchiveInfo), &EditArchiveInfo);
                 spl->IncProgress("TZX Manager"); Application->CreateForm(__classid(TEditTextForm), &EditTextForm);
                 spl->IncProgress("TZX Manager"); Application->CreateForm(__classid(TEditHWInfoForm), &EditHWInfoForm);
                 spl->IncProgress("TZX Manager"); Application->CreateForm(__classid(TEditDataForm), &EditDataForm);
                 spl->IncProgress("TZX Manager"); Application->CreateForm(__classid(TFSSettings), &FSSettings);
                 spl->IncProgress("TZX Manager"); Application->CreateForm(__classid(TEditGeneralForm), &EditGeneralForm);
                 spl->IncProgress("Wav Loader"); Application->CreateForm(__classid(TWavLoad), &WavLoad);
                 spl->IncProgress("ZIP Support"); Application->CreateForm(__classid(TZipFile), &ZipFile);
                 spl->IncProgress("Basic Loader"); Application->CreateForm(__classid(TLoadBasicListingOptionsForm), &LoadBasicListingOptionsForm);
                 spl->IncProgress("Basic Loader"); Application->CreateForm(__classid(TSaveBasicListingOptionsForm), &SaveBasicListingOptionsForm);
                 spl->IncProgress("Basic Lister"); Application->CreateForm(__classid(TBasicLister), &BasicLister);
                 spl->IncProgress("Basic Lister"); Application->CreateForm(__classid(TBasicListingFormatInfoForm), &BasicListingFormatInfoForm);
                 spl->IncProgress("TV Emulation"); Application->CreateForm(__classid(TArtifacts), &Artifacts);
                 spl->IncProgress("ZX97"); Application->CreateForm(__classid(TZX97Dialog), &ZX97Dialog);
                 spl->IncProgress("ZX Interface 1"); Application->CreateForm(__classid(TIF1), &IF1);
                 spl->IncProgress("Floppy Disk"); Application->CreateForm(__classid(TP3Drive), &P3Drive);
                 spl->IncProgress("Hard Disk"); Application->CreateForm(__classid(TCreateHDF), &CreateHDF);
                 spl->IncProgress("Serial Port"); Application->CreateForm(__classid(TSerialConfig), &SerialConfig);
                 spl->IncProgress("Parallel Port"); Application->CreateForm(__classid(TParallelPort), &ParallelPort);
                 spl->IncProgress("Sound"); Application->CreateForm(__classid(TSoundOutput), &SoundOutput);
                 spl->IncProgress("Midi"); Application->CreateForm(__classid(TMidiForm), &MidiForm);
                 spl->IncProgress("ZX Printer"); Application->CreateForm(__classid(TZXPrinter), &ZXPrinter);

                 // Always do this last
                 spl->IncProgress("Machine Emulation"); Application->CreateForm(__classid(THW), &HW);

                 spl->Close();

                 Form1->Enabled = true;

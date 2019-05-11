/*
 ==============================================================================

 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.

 See LICENSE.txt for  more info.

 ==============================================================================
*/

#pragma once

/**
 * @file
 * @brief This file contains the base IControl implementation, along with some base classes for specific types of control.
 */

#include <cstring>
#include <cstdlib>
#include <vector>

#if defined VST3_API || defined VST3C_API
#undef stricmp
#undef strnicmp
#include "pluginterfaces/vst/ivstcontextmenu.h"
#include "base/source/fobject.h"
#endif

#include "IPlugPlatform.h"

#include "wdlstring.h"
#include "ptrlist.h"

#include "IGraphics.h"

/** The lowest level base class of an IGraphics control. A control is anything on the GUI 
*  @ingroup BaseControls */
class IControl
#if defined VST3_API || defined VST3C_API
: public Steinberg::Vst::IContextMenuTarget
, public Steinberg::FObject
#endif
{
public:
  /** Constructor
   * @brief Creates an IControl
   * NOTE: An IControl does not know about the delegate or graphics context to which it belongs in the constructor
   * If you need to do something once those things are known, see IControl::OnInit()
   * @param bounds The rectangular area that the control occupies
   * @param paramIdx If this is > -1 (kNoParameter) this control will be associated with a plugin parameter
   * @param actionFunc pass in a lambda function to provide custom functionality when the control "action" happens (usually mouse down). */
  IControl(IRECT bounds, int paramIdx = kNoParameter, IActionFunction actionFunc = nullptr);
  
  /** Constructor (range of parameters)
   * @brief Creates an IControl which is linked to multiple parameters
   * NOTE: An IControl does not know about the delegate or graphics context to which it belongs in the constructor
   * If you need to do something once those things are know, see IControl::OnInit()
   * @param bounds The rectangular area that the control occupies
   * @param params An initializer list of valid integer parameter indexes
   * @param actionFunc pass in a lambda function to provide custom functionality when the control "action" happens (usually mouse down). */
  IControl(IRECT bounds, const std::initializer_list<int>& params, IActionFunction actionFunc = nullptr);
  
  /** Constructor (no paramIdx)
   * @brief Creates an IControl which is not linked to a parameter
   * NOTE: An IControl does not know about the delegate or graphics context to which it belongs in the constructor
   * If you need to do something once those things are known, see IControl::OnInit()
   * @param bounds The rectangular area that the control occupies
   * @param actionFunc pass in a lambda function to provide custom functionality when the control "action" happens (usually mouse down). */
  IControl(IRECT bounds, IActionFunction actionFunc);

  /** Destructor. Clean up any resources that your control owns. */
  virtual ~IControl() {}

  /** Implement this method to respond to a mouse down event on this control. 
   * @param x The X coordinate of the mouse event
   * @param y The Y coordinate of the mouse event
   * @param mod A struct indicating which modifier keys are held for the event */
  virtual void OnMouseDown(float x, float y, const IMouseMod& mod);

/** Implement this method to respond to a mouse up event on this control. 
   * @param x The X coordinate of the mouse event
   * @param y The Y coordinate of the mouse event
   * @param mod A struct indicating which modifier keys are held for the event */
  virtual void OnMouseUp(float x, float y, const IMouseMod& mod) {}

  /** Implement this method to respond to a mouse drag event on this control. 
   * @param x The X coordinate of the mouse event
   * @param y The Y coordinate of the mouse event
   * @param dX The X delta (difference) since the last event
   * @param dY The Y delta (difference) since the last event
   * @param mod A struct indicating which modifier keys are held for the event */
  virtual void OnMouseDrag(float x, float y, float dX, float dY, const IMouseMod& mod) {}
   
  /** Implement this method to respond to a mouse double click event on this control. 
   * @param x The X coordinate of the mouse event
   * @param y The Y coordinate of the mouse event
   * @param mod A struct indicating which modifier keys are held for the event */
  virtual void OnMouseDblClick(float x, float y, const IMouseMod& mod);

  /** Implement this method to respond to a mouse wheel event on this control. 
   * @param x The X coordinate of the mouse event
   * @param y The Y coordinate of the mouse event
   * @param mod A struct indicating which modifier keys are held for the event 
   * @param d The delta value (difference) since the last mouse wheel event */
  virtual void OnMouseWheel(float x, float y, const IMouseMod& mod, float d) {};

  /** Implement this method to respond to a key down event on this control. 
   * @param x The X coordinate of the mouse at the time of this key down event
   * @param y The Y coordinate of the mouse at the time of this key down event
   * @param key \todo */
  virtual bool OnKeyDown(float x, float y, const IKeyPress& key) { return false; }

  /** Implement this method to respond to a key up event on this control.
   * @param x The X coordinate of the mouse at the time of this key down event
   * @param y The Y coordinate of the mouse at the time of this key down event
   * @param key \todo */
  virtual bool OnKeyUp(float x, float y, const IKeyPress& key) { return false; }
  
  /** Implement this method to respond to a mouseover event on this control. Implementations should call base class, if you wish to use mMouseIsOver.
   * @param x The X coordinate of the mouse event
   * @param y The Y coordinate of the mouse event
   * @param mod A struct indicating which modifier keys are held for the event */
  virtual void OnMouseOver(float x, float y, const IMouseMod& mod);

  /** Implement this method to respond to a mouseout event on this control. Implementations should call base class, if you wish to use mMouseIsOver. */
  virtual void OnMouseOut();

  /** Implement to do something when something was drag 'n dropped onto this control */
  virtual void OnDrop(const char* str) {};

  /** Implement to do something when graphics is scaled globally (e.g. moves to high DPI screen) */
  virtual void OnRescale() {}

  /** Called when IControl is constructed or resized using SetRect(). NOTE: if you call SetDirty() in this method, you should call SetDirty(false) to avoid triggering parameter changes */
  virtual void OnResize() {}
  
  /** Called after the control has been attached, and its delegate and graphics member variable set */
  virtual void OnInit() {}
  
  /** Implement to receive messages sent to the control, see IEditorDelegate:SendControlMsgFromDelegate() */
  virtual void OnMsgFromDelegate(int messageTag, int dataSize, const void* pData) {};
  
  /** Implement to receive MIDI messages sent to the control if mWantsMidi == true, see IEditorDelegate:SendMidiMsgFromDelegate() */
  virtual void OnMidi(const IMidiMsg& msg) {};

  /** Called by default when the user right clicks a control. If IGRAPHICS_NO_CONTEXT_MENU is enabled as a preprocessor macro right clicking control will mean IControl::CreateContextMenu() and IControl::OnContextSelection() do not function on right clicking control. VST3 provides contextual menu support which is hard wired to right click controls by default. You can add custom items to the menu by implementing IControl::CreateContextMenu() and handle them in IControl::OnContextSelection(). In non-VST 3 hosts right clicking will still create the menu, but it will not feature entries added by the host. */
  virtual void CreateContextMenu(IPopupMenu& contextMenu) {}
  
  /** Implement this method to handle popup menu selection after IGraphics::CreatePopupMenu/IControl::PromptUserInput
   * @param pSelectedMenu If pSelectedMenu is invalid it means the user didn't select anything
   * @param valIdx An index that indicates which of the controls vals the menu relates to */
  virtual void OnPopupMenuSelection(IPopupMenu* pSelectedMenu, int valIdx);

  /** Implement this method to handle text input after IGraphics::CreateTextEntry/IControl::PromptUserInput
   * @param str A CString with the inputted text
   * @param valIdx An index that indicates which of the controls vals the text completion relates to */
  virtual void OnTextEntryCompletion(const char* str, int valIdx) {}

  /** Implement this to respond to a menu selection from CreateContextMenu(); @see CreateContextMenu() */
  virtual void OnContextSelection(int itemSelected) {}

  /** Draw the control to the graphics context. 
   * @param g The graphics context to which this control belongs. */
  virtual void Draw(IGraphics& g) = 0;

  /** Implement this to customise how a colored highlight is drawn on the control in ProTools (AAX format only), when a control is linked to a parameter that is automated.
   * @param g The graphics context to which this control belongs. */
  virtual void DrawPTHighlight(IGraphics& g);

  /** Call this method in response to a mouse event to create an edit box so the user can enter a value, or pop up a pop-up menu,
   * if the control is linked to a parameter (mParamIdx > kNoParameter)
   * @param valIdx An index to choose which of the controls linked parameters to retrieve. NOTE: since controls usually have only 1 parameter you can omit this argument and use the default index of 0 */
  void PromptUserInput(int valIdx = 0);
  
  /** Create a text entry box so the user can enter a value, or pop up a pop-up menu,
   * if the control is linked to a parameter (mParamIdx > kNoParameter), specifying the bounds
   * @param bounds The rectangle for the text entry. Pop-up menu's will appear below the rectangle. /todo check
   * @param valIdx An index to choose which of the controls linked parameters to retrieve. NOTE: since controls usually have only 1 parameter you can omit this argument and use the default index of 0 */
  void PromptUserInput(const IRECT& bounds, int valIdx = 0);
  
  /** Set an Action Function for this control. 
   * actionfunc @see Action Functions */
  inline void SetActionFunction(IActionFunction actionFunc) { mActionFunc = actionFunc; }

  /** Set a tooltip for the control
   * @param str CString tooltip to be displayed */
  inline void SetTooltip(const char* str) { mTooltip.Set(str); }
  
  /** @return Currently set tooltip text */
  inline const char* GetTooltip() const { return mTooltip.Get(); }

  /** Get the index of a parameter that the control is linked to
   * Normaly controls are either linked to a single parameter or no parameter but some may be linked to multiple parameters
   * @param valIdx An index to choose which of the controls linked parameters to retrieve. NOTE: since controls usually have only 1 parameter you can omit this argument and use the default index of 0
   * @return Parameter index, or kNoParameter if there is no parameter linked with this control at valIdx */
  int GetParamIdx(int valIdx = 0) const;
  
  /** Set the index of a parameter that the control is linked to
   * @param paramIdx Parameter index, or kNoParameter if there is no parameter linked with this control at valIdx
   * @param valIdx An index to choose which of the controls vals to set */
  void SetParamIdx(int paramIdx, int valIdx = 0);
 
  /** Check if the control is linked to a particular parameter
   * @param paramIdx The paramIdx to test
   * @return the valIdx if linked, or kNoValIdx if not */
  int LinkedToParam(int paramIdx) const;
  
  /** @return The number of values for this control */
  int NVals() const { return (int) mVals.size(); }

  /** Check to see which of the control's values relates to this x and y coordinate
   * @param x x coordinate to check
   * @param y x coordinate to check
   * @return An integer specifying which value matches the x, y coordinates, or kNoValIdx if the position is not linked to a value. */
  virtual int GetValIdxForPos(float x, float y) const { return mVals.size() == 1 ? 0 : kNoValIdx; }
  
  /** Get a const pointer to the IParam object (owned by the editor delegate class), associated with this control
   * @return const pointer to an IParam or nullptr if the control is not associated with a parameter */ 
  const IParam* GetParam(int valIdx = 0);
  
  /** Set the control's value from the delegate
   * This method is called from the class implementing the IEditorDelegate interface in order to update a control's value members and set it to be marked dirty for redraw.
   * @param value Normalised incoming value
   * @param valIdx The index of the value to set, which should be between 0 and NVals() */
  virtual void SetValueFromDelegate(double value, int valIdx = 0);
  
  /** Set the control's value after user input.
   * This method is called after a text entry or popup menu prompt triggered by PromptUserInput(), calling SetDirty(true), which will mean that the new value gets sent back to the delegate
   * @param value the normalised value after user input via text entry or pop-up menu
   * @param valIdx An index to choose which of the controls linked parameters to retrieve. NOTE: since controls usually have only 1 parameter you can omit this argument and use the default index of 0 */
  virtual void SetValueFromUserInput(double value, int valIdx = 0);
    
  /** Set one or all of the control's values to the default value of the associated parameter.
   * @param valIdx either an integer > -1 (kNoValIdx) in order to set an individual value to the default value of the associated parameter, or kNoValIdx to default all values
   * This method will call through to SetDirty(true, valIdx), which will mean that the new value gets sent back to the delegate */
  virtual void SetValueToDefault(int valIdx = kNoValIdx);
  
  /** Set one of the control's values.
   * @param value The normalized 0-1 value
   * @param valIdx The index of the value to set, which should be between 0 and NVals() */
  virtual void SetValue(double value, int valIdx = 0);
  
  /** Get the control's value
   * @return Value of the control, normalized in the range 0-1
   * @param valIdx The index of the value to set, which should be between 0 and NVals() */
  double GetValue(int valIdx = 0) const;
  
  /** Assign the control to a control group @see Control Groups
   * @param groupName A CString indicating the control group that this control should belong to */
  void SetGroup(const char* groupName) { mGroup.Set(groupName); }
  
  /** Get the group that the control belongs to, if any
   * @return A CString indicating the control group that this control belongs to (may be empty) */
  const char* GetGroup() const { return mGroup.Get(); }

  /** Get the Text object for the control
   * @return const IText& The control's mText object, typically used to determine font/layout/size etc of the main text in a control. */
  const IText& GetText() const { return mText; }

  /** Set the Text object typically used to determine font/layout/size etc of the main text in a control
   * @param txt An IText struct with the desired formatting */
  void SetText(const IText& txt) { mText = txt; }

  /** Get the max number of characters that are allowed in text entry 
   * @return int The max number of characters allowed in text entry */
  int GetTextEntryLength() const { return mTextEntryLength; }

  /** Set the max number of characters that are allowed in text entry 
   * @param len The max number of characters allowed in text entry */
  void SetTextEntryLength(int len) { mTextEntryLength = len;  }
  
  /** Get the rectangular draw area for this control, within the graphics context
   * @return The control's bounds */
  const IRECT& GetRECT() const { return mRECT; }

  /** Set the rectangular draw area for this control, within the graphics context
   * @param bounds The control's bounds */
  void SetRECT(const IRECT& bounds) { mRECT = bounds; mMouseIsOver = false; OnResize(); }
  
  /** Get the rectangular mouse tracking target area, within the graphics context for this control
   * @return The control's target bounds within the graphics context */
  const IRECT& GetTargetRECT() const { return mTargetRECT; } // The mouse target area (default = draw area).

  /** Set the rectangular mouse tracking target area, within the graphics context for this control
   * @param bounds The control's new target bounds within the graphics context */
  void SetTargetRECT(const IRECT& bounds) { mTargetRECT = bounds; mMouseIsOver = false; }
  
  /** Set BOTH the draw rect and the target area, within the graphics context for this control
   * @param bounds The control's new draw and target bounds within the graphics context */
  void SetTargetAndDrawRECTs(const IRECT& bounds) { mRECT = mTargetRECT = bounds; mMouseIsOver = false; OnResize(); }

  /** Used internally by the AAX wrapper view interface to set the control parmeter highlight 
   * @param isHighlighted /c true if the control should be highlighted 
   * @param color An integer representing one of three colors that ProTools assigns automated controls */
  void SetPTParameterHighlight(bool isHighlighted, int color);
  
  /** Get double click as single click 
   * By default, mouse double click has its own handler. A control can set mDblAsSingleClick to true 
   * which maps double click to single click for this control (and also causes the mouse to be captured by the control on double click).
   * @return /c true if double clicks should be mapped to single clicks */
  bool GetMouseDblAsSingleClick() { return mDblAsSingleClick; }

  /** Shows or hides the IControl.
   * @param hide Set to \c true to hide the control */
  virtual void Hide(bool hide);
  
  /** @return \c true if the control is hidden. */
  bool IsHidden() const { return mHide; }

  /** Sets gray out mode for the control
   * @param gray \c true for grayed out*/
  virtual void GrayOut(bool gray);
  
  /** @return \c true if the control is grayed */
  bool IsGrayed() const { return mGrayed; }

  /** Specify whether the control should respond to mouse overs when grayed out
   * @param allow \c true if it should respond to mouse overs when grayed out (false by default) */
  void SetMOWhenGrayed(bool allow) { mMOWhenGrayed = allow; }

  /** Specify whether the control should respond to other mouse events when grayed out
   * @param allow \c true if it should respond to other mouse events when grayed out (false by default) */
  void SetMEWhenGrayed(bool allow) { mMEWhenGrayed = allow; }

  /** @return \c true if the control responds to mouse overs when grayed out */
  bool GetMOWhenGrayed() const { return mMOWhenGrayed; }

  /** @return \c true if the control responds to other mouse events when grayed out */
  bool GetMEWhenGrayed() const { return mMEWhenGrayed; }
  
  /** @return \c true if the control ignores mouse events */
  bool GetIgnoreMouse() const { return mIgnoreMouse; }

  /** Hit test the control. Override this method if you want the control to be hit only if a visible part of it is hit, or whatever.
   * @param x The X coordinate within the control to test 
   * @param y The y coordinate within the control to test
   * @return \c Return true if the control was hit. */
  virtual bool IsHit(float x, float y) const { return mTargetRECT.Contains(x, y); }

  /** Mark the control as dirty, i.e. it should be redrawn on the next display refresh
   * @param triggerAction If this is true and the control is linked to a parameter
   * notify the class implementing the IEditorDelegate interface that the parameter changed. If this control has an ActionFunction, that can also be triggered.
   * NOTE: it is easy to forget that this method always sets the control dirty, the argument is about whether a consecutive action should be performed */
  virtual void SetDirty(bool triggerAction = true, int valIdx = kNoValIdx);

  /* Set the control clean, i.e. Called by IGraphics draw loop after control has been drawn */
  virtual void SetClean() { mDirty = false; }
  
  /** Called at each display refresh by the IGraphics draw loop to determine if the control is marked as dirty. 
   * This is not const, because it is typically  overridden and used to update something at the display refresh rate
   * The default implementation executes a control's Animation Function, so if you override this you may want to call the base implementation, @see Animation Functions
   * @return \c true if the control is marked dirty. */
  virtual bool IsDirty();

  /** Disable/enable right-clicking the control to prompt for user input /todo check this
   * @param disable \c true*/
  void DisablePrompt(bool disable) { mDisablePrompt = disable; }

  /** This is an idle call from the GUI thread, only active if USE_IDLE_CALLS is defined. /todo check this */
  virtual void OnGUIIdle() {}
  
  /** Set the control's tag. Controls can be given tags, in order to direct messages to them. @see Control Tags
   * @param tag A unique integer to identify this control */
  void SetTag(int tag) { mTag = tag; }
  
  /** Get the control's tag. @see Control Tags */
  int GetTag() const { return mTag; }
  
  /** Specify whether this control wants to know about MIDI messages sent to the UI. See OnMIDIMsg() */
  void SetWantsMidi(bool enable) { mWantsMidi = true; }

  /** @return /c true if this control wants to know about MIDI messages send to the UI. See OnMIDIMsg() */
  bool GetWantsMidi() const { return mWantsMidi; }

  /** Gets a pointer to the class implementing the IEditorDelegate interface that handles parameter changes from this IGraphics instance.
   * If you need to call other methods on that class, you can use static_cast<PLUG_CLASS_NAME>(GetDelegate();
   * @return The class implementing the IEditorDelegate interface that handles communication to/from from this IGraphics instance.*/
  IEditorDelegate* GetDelegate() { return mDelegate; }
  
  /** Used internally to set the mDelegate (and mGraphics) variables */
  void SetDelegate(IGEditorDelegate& dlg)
  {
    mDelegate = &dlg;
    mGraphics = dlg.GetUI();
    OnInit();
    OnResize();
    OnRescale();
  }
  
  /** @return A pointer to the IGraphics context that owns this control */ 
  IGraphics* GetUI() { return mGraphics; }
    
  /** @return A const pointer to the IGraphics context that owns this control */
  const IGraphics* GetUI() const { return mGraphics; }

  /* This can be used in IControl::Draw() to check if the mouse is over the control, without implementing mouse over methods 
   * @return \true if the mouse is over this control. */
  bool GetMouseIsOver() const { return mMouseIsOver; }
  
  /** Set control value based on x, y position within a rectangle. Commonly used for slider/fader controls.
   * @param x The X coordinate for snapping
   * @param y The Y coordinate for snapping
   * @param direction The direction of the control's travel- horizontal or vertical fader
   * @param bounds The area in which the track of e.g. a slider should be snapped
   * @param valIdx /todo
   * @param scalar A scalar to speedup/slowdown mousing along the track */
  virtual void SnapToMouse(float x, float y, EDirection direction, IRECT& bounds, int valIdx = -1, float scalar = 1.);

  virtual void OnEndAnimation() // if you override this you must call the base implementation, to free mAnimationFunc
  {
    mAnimationFunc = nullptr;
    SetDirty(false);
  }
  
  /** @param duration Duration in milliseconds for the animation  */
  void StartAnimation(int duration)
  {
    mAnimationStartTime = std::chrono::high_resolution_clock::now();
    mAnimationDuration = Milliseconds(duration);
  }
  
  /** Set the animation function
   * @param func A std::function conforming to IAnimationFunction */
  void SetAnimation(IAnimationFunction func) { mAnimationFunc = func;}
  
  /** Set the animation function and starts it
   * @param func A std::function conforming to IAnimationFunction
   * @param duration Duration in milliseconds for the animation  */
  void SetAnimation(IAnimationFunction func, int duration) { mAnimationFunc = func; StartAnimation(duration); }

  IAnimationFunction GetAnimationFunction() { return mAnimationFunc; }
  
  IAnimationFunction GetActionFunction() { return mActionFunc; }

  double GetAnimationProgress()
  {
    if(!mAnimationFunc)
      return 0.;
    
    auto elapsed = Milliseconds(Time::now() - mAnimationStartTime);
    return elapsed.count() / mAnimationDuration.count();
  }
  
#if defined VST3_API || defined VST3C_API
  Steinberg::tresult PLUGIN_API executeMenuItem (Steinberg::int32 tag) override { OnContextSelection(tag); return Steinberg::kResultOk; }
#endif
  
#pragma mark - IControl Member variables
protected:
  
  /** A helper template function to call a method for an individual value, or for all values
   * @param valIdx If this is > kNoValIdx execute the function for an individual value. If equal to kNoValIdx call the function for all values
   * @param func A function that takes a single integer argument, the valIdx \todo
   * @param args Arguments to the function */
  template<typename T, typename... Args>
  void ForValIdx(int valIdx, T func, Args... args)
  {
    if (valIdx > kNoValIdx)
      func(valIdx, args...);
    else
    {
      const int nVals = NVals();
      for (int v = 0; v < nVals; v++)
        func(v, args...);
    }
  }
    
  int mTag = kNoTag;
  IRECT mRECT;
  IRECT mTargetRECT;
  
  /** Controls can be grouped for hiding and showing panels */
  WDL_String mGroup;
  
  IText mText;

  int mTextEntryLength = DEFAULT_TEXT_ENTRY_LEN;
  bool mDirty = true;
  bool mHide = false;
  bool mGrayed = false;
  bool mDisablePrompt = true;
  bool mDblAsSingleClick = false;
  bool mMOWhenGrayed = false;
  bool mMEWhenGrayed = false;
  bool mIgnoreMouse = false;
  bool mWantsMidi = false;
  /** if mGraphics::mHandleMouseOver = true, this will be true when the mouse is over control. If you need finer grained control of mouseovers, you can override OnMouseOver() and OnMouseOut() */
  bool mMouseIsOver = false;
  WDL_String mTooltip;

  IColor mPTHighlightColor = COLOR_RED;
  bool mPTisHighlighted = false;
  
  void SetNVals(int nVals)
  {
    assert(nVals > 0);
    mVals.resize(nVals);
  }

#if defined VST3_API || defined VST3C_API
  OBJ_METHODS(IControl, FObject)
  DEFINE_INTERFACES
  DEF_INTERFACE (IContextMenuTarget)
  END_DEFINE_INTERFACES (FObject)
  REFCOUNT_METHODS(FObject)
#endif
  
private:
  IEditorDelegate* mDelegate = nullptr;
  IGraphics* mGraphics = nullptr;
  IActionFunction mActionFunc = nullptr;
  IAnimationFunction mAnimationFunc = nullptr;
  TimePoint mAnimationStartTime;
  Milliseconds mAnimationDuration;
  std::vector<ParamTuple> mVals { {kNoParameter, 0.} };
};

#pragma mark - Base Controls

/**
 * \addtogroup BaseControls
 * @{
 */

/** A base interface, to be combined with IControl for bitmap-based controls "IBControls", managing an IBitmap and IBlend */
class IBitmapBase
{
public:
  IBitmapBase(const IBitmap& bitmap, EBlendType blend = kBlendDefault)
  : mBitmap(bitmap)
  , mBlend(blend)
  {
  }
  
  virtual ~IBitmapBase() {}
  
  void GrayOut(bool gray)
  {
    mBlend.mWeight = (gray ? GRAYED_ALPHA : 1.0f);
  }
  
  void SetBlend(const IBlend& blend)
  {
    mBlend = blend;
  }

protected:
  IBitmap mBitmap;
  IBlend mBlend;
};

/** A base interface to be combined with IControl for vectorial controls "IVControls", in order for them to share a common set of colors. 
 * If you need more flexibility, you're on your own! */
class IVectorBase
{
public:
  IVectorBase(const IColor* pBGColor = &DEFAULT_BGCOLOR,
              const IColor* pFGColor = &DEFAULT_FGCOLOR,
              const IColor* pPRColor = &DEFAULT_PRCOLOR,
              const IColor* pFRColor = &DEFAULT_FRCOLOR,
              const IColor* pHLColor = &DEFAULT_HLCOLOR,
              const IColor* pSHColor = &DEFAULT_SHCOLOR,
              const IColor* pX1Color = &DEFAULT_X1COLOR,
              const IColor* pX2Color = &DEFAULT_X2COLOR,
              const IColor* pX3Color = &DEFAULT_X3COLOR)
  {
    AddColors(pBGColor, pFGColor, pPRColor, pFRColor, pHLColor, pSHColor, pX1Color, pX2Color, pX3Color);
  }

  IVectorBase(const IVColorSpec& spec)
  {
    AddColors(&spec.mBGColor,
              &spec.mFGColor,
              &spec.mPRColor,
              &spec.mFRColor,
              &spec.mHLColor,
              &spec.mSHColor,
              &spec.mX1Color,
              &spec.mX2Color,
              &spec.mX3Color);
  }
  
  void AttachIControl(IControl* pControl) { mControl = pControl; }
  
  void AddColor(const IColor& color)
  {
    mColors.Add(color);
  }
  
  void AddColors(const IColor* pBGColor = 0,
                 const IColor* pFGColor = 0,
                 const IColor* pPRColor = 0,
                 const IColor* pFRColor = 0,
                 const IColor* pHLColor = 0,
                 const IColor* pSHColor = 0,
                 const IColor* pX1Color = 0,
                 const IColor* pX2Color = 0,
                 const IColor* pX3Color = 0)
  {
    if(pBGColor) AddColor(*pBGColor);
    if(pFGColor) AddColor(*pFGColor);
    if(pPRColor) AddColor(*pPRColor);
    if(pFRColor) AddColor(*pFRColor);
    if(pHLColor) AddColor(*pHLColor);
    if(pSHColor) AddColor(*pSHColor);
    if(pX1Color) AddColor(*pX1Color);
    if(pX2Color) AddColor(*pX2Color);
    if(pX3Color) AddColor(*pX3Color);
  }

  void SetColor(int colorIdx, const IColor& color)
  {
    if(colorIdx < mColors.GetSize())
      mColors.Get()[colorIdx] = color;
    
    mControl->SetDirty(false);
  }
  
  void SetColors(const IColor& BGColor,
                 const IColor& FGColor,
                 const IColor& PRColor,
                 const IColor& FRColor,
                 const IColor& HLColor,
                 const IColor& SHColor,
                 const IColor& X1Color,
                 const IColor& X2Color,
                 const IColor& X3Color)
  {
    mColors.Get()[kBG] = BGColor;
    mColors.Get()[kFG] = FGColor;
    mColors.Get()[kPR] = PRColor;
    mColors.Get()[kFR] = FRColor;
    mColors.Get()[kHL] = HLColor;
    mColors.Get()[kSH] = SHColor;
    mColors.Get()[kX1] = X1Color;
    mColors.Get()[kX2] = X2Color;
    mColors.Get()[kX3] = X3Color;
    
    mControl->SetDirty(false);
  }

  void SetColors(const IVColorSpec& spec)
  {
    SetColors(spec.mBGColor,
              spec.mFGColor,
              spec.mPRColor,
              spec.mFRColor,
              spec.mHLColor,
              spec.mSHColor,
              spec.mX1Color,
              spec.mX2Color,
              spec.mX3Color);
  }

  IColor& GetColor(int colorIdx)
  {
    if(colorIdx < mColors.GetSize())
      return mColors.Get()[colorIdx];
    else
      return mColors.Get()[0];
  }
  
  void SetRoundness(float roundness) { mRoundness = Clip(roundness, 0.f, 1.f); mControl->SetDirty(false); }
  void SetDrawFrame(bool draw) { mDrawFrame = draw; mControl->SetDirty(false); }
  void SetDrawShadows(bool draw) { mDrawShadows = draw; mControl->SetDirty(false); }
  void SetEmboss(bool emboss) { mEmboss = emboss; mControl->SetDirty(false); }
  void SetShadowOffset(float offset) { mShadowOffset = offset; mControl->SetDirty(false); }
  void SetFrameThickness(float thickness) { mFrameThickness = thickness; mControl->SetDirty(false); }
  void SetSplashRadius(float radius) { mSplashRadius = radius * mMaxSplashRadius; }

  void Style(bool drawFrame, bool drawShadows, bool emboss, float roundness, float frameThickness, float shadowOffset, const IVColorSpec& spec)
  {
    mDrawFrame = drawFrame;
    mDrawShadows = drawShadows;
    mEmboss = emboss;
    mRoundness = roundness;
    mFrameThickness = frameThickness;
    mShadowOffset = shadowOffset;
    SetColors(spec);
  }
  
  IRECT GetAdjustedHandleBounds(IRECT handleBounds)
  {
    if(mDrawFrame)
      handleBounds.Pad(- 0.5f * mFrameThickness);
    
    if (mDrawShadows && !mEmboss)
      handleBounds.Alter(0, 0, -mShadowOffset, -mShadowOffset);
    
    return handleBounds;
  }
  
  void DrawSplash(IGraphics& g)
  {
    float mouseDownX, mouseDownY;
    g.GetMouseDownPoint(mouseDownX, mouseDownY);
    g.FillCircle(GetColor(kHL), mouseDownX, mouseDownY, mSplashRadius);
  }
  
  IRECT DrawVectorButton(IGraphics&g, const IRECT& bounds, bool pressed, bool mouseOver)
  {
    g.FillRect(GetColor(kBG), bounds);
    
    IRECT handleBounds = GetAdjustedHandleBounds(bounds);
    const float cornerRadius = mRoundness * (handleBounds.W() / 2.f);
    
    if (pressed)
    {
      g.FillRoundRect(GetColor(kPR), handleBounds, cornerRadius);
      
      //inner shadow
      if (mDrawShadows && mEmboss)
      {
        g.PathRect(handleBounds.GetHSliced(mShadowOffset));
        g.PathRect(handleBounds.GetVSliced(mShadowOffset));
        g.PathFill(GetColor(kSH));
      }
    }
    else
    {
      //outer shadow
      if (mDrawShadows && !mEmboss)
        g.FillRoundRect(GetColor(kSH), handleBounds.GetTranslated(mShadowOffset, mShadowOffset), cornerRadius);
      
      g.FillRoundRect(GetColor(kFG), handleBounds, cornerRadius);
    }
    
    if(mouseOver)
      g.FillRoundRect(GetColor(kHL), handleBounds, cornerRadius);
    
    if(mControl->GetAnimationFunction())
      DrawSplash(g);
    
    if(mDrawFrame)
      g.DrawRoundRect(GetColor(kFR), handleBounds, cornerRadius, 0, mFrameThickness);
    
    return handleBounds;
  }
  
protected:
  IControl* mControl = nullptr;
  WDL_TypedBuf<IColor> mColors;
  float mRoundness = 0.f;
  float mShadowOffset = 3.f;
  float mFrameThickness = 2.f;
  bool mDrawFrame = true;
  bool mDrawShadows = true;
  bool mEmboss = false;
  float mSplashRadius = 0.f;
  float mMaxSplashRadius = 50.f;
};

/** A base class for knob/dial controls, to handle mouse action and ballistics. */
class IKnobControlBase : public IControl
{
public:
  IKnobControlBase(IRECT bounds, int paramIdx = kNoParameter,
    EDirection direction = kVertical, double gearing = DEFAULT_GEARING)
    : IControl(bounds, paramIdx)
    , mDirection(direction)
    , mGearing(gearing)
  {}

  void SetGearing(double gearing) { mGearing = gearing; }
  bool IsFineControl(const IMouseMod& mod, bool wheel) const;
  void OnMouseDrag(float x, float y, float dX, float dY, const IMouseMod& mod) override;
  void OnMouseWheel(float x, float y, const IMouseMod& mod, float d) override;

protected:
  EDirection mDirection;
  double mGearing;
};

/** A base class for slider/fader controls, to handle mouse action and ballistics. */
class ISliderControlBase : public IControl
{
public:
  ISliderControlBase(IRECT bounds, int paramIdx = kNoParameter,  EDirection dir = kVertical, bool onlyHandle = false, float handleSize = 0.f);
  ISliderControlBase(IRECT bounds, IActionFunction aF = nullptr, EDirection dir = kVertical, bool onlyHandle = false, float handleSize = 0.f);
  
  void OnMouseDown(float x, float y, const IMouseMod& mod) override { SnapToMouse(x, y, mDirection, mTrack); }
  void OnMouseDrag(float x, float y, float dX, float dY, const IMouseMod& mod) override { SnapToMouse(x, y, mDirection, mTrack); }
  
protected:
  EDirection mDirection;
  IRECT mTrack;
  bool mOnlyHandle;
  float mHandleSize;
};

/** A base class for mult-strip/track controls, such as multi-sliders, meters */
class IVTrackControlBase : public IControl
                         , public IVectorBase
{
public:
  IVTrackControlBase(IRECT bounds, int maxNTracks = 1, EDirection dir = kHorizontal, float minTrackValue = 0.f, float maxTrackValue = 1.f, const char* trackNames = 0, ...)
  : IControl(bounds)
  , mMinTrackValue(minTrackValue)
  , mMaxTrackValue(maxTrackValue)
  , mDirection(dir)
  {
    SetNVals(maxNTracks);

    for (int i=0; i<maxNTracks; i++)
    {
      SetParamIdx(kNoParameter, i);
      mTrackBounds.Add(IRECT());
    }
    
    AttachIControl(this);
  }

  IVTrackControlBase(IRECT bounds, int lowParamidx, int maxNTracks = 1, EDirection dir = kHorizontal, float minTrackValue = 0.f, float maxTrackValue = 1.f, const char* trackNames = 0, ...)
    : IControl(bounds)
    , mMinTrackValue(minTrackValue)
    , mMaxTrackValue(maxTrackValue)
    , mDirection(dir)
  {
    SetNVals(maxNTracks);

    for (int i = 0; i < maxNTracks; i++)
    {
      SetParamIdx(lowParamidx+i, i);
      mTrackBounds.Add(IRECT());
    }

    AttachIControl(this);
  }
  
  void MakeRects()
  {
    int nVals = NVals();
    for (int ch = 0; ch < nVals; ch++)
    {
      mTrackBounds.Get()[ch] = mRECT.GetPadded(-mOuterPadding).
                                     SubRect(EDirection(!mDirection), nVals, ch).
                                     GetPadded(0, -mTrackPadding * (float) mDirection, -mTrackPadding * (float) !mDirection, -mTrackPadding);
    }
  }
  
  void Draw(IGraphics& g) override
  {
    int nVals = NVals();

    g.FillRect(GetColor(kBG), mRECT);
    
    for (int ch = 0; ch < nVals; ch++)
    {
      DrawTrack(g, mTrackBounds.Get()[ch], ch);
    }
    
    if(mDrawFrame)
      DrawFrame(g);
  }
  
  //void SetAllTrackData(float val) { memset(mTrackData.Get(), (int) Clip(val, mMinTrackValue, mMaxTrackValue), mTrackData.GetSize() * sizeof(float) ); }
private:
  virtual void DrawFrame(IGraphics& g)
  {
    g.DrawRect(GetColor(kFR), mRECT, nullptr, mFrameThickness);
  }
  
  virtual void DrawTrack(IGraphics& g, IRECT& r, int chIdx)
  {
    DrawTrackBG(g, r, chIdx);
    DrawTrackHandle(g, r, chIdx);
    
    if(mDrawTrackFrame)
      g.DrawRect(GetColor(kFR), r, nullptr, mFrameThickness);
  }
  
  virtual void DrawTrackBG(IGraphics& g, IRECT& r, int chIdx)
  {
    g.FillRect(GetColor(kSH), r);
  }
  
  virtual void DrawTrackHandle(IGraphics& g, IRECT& r, int chIdx)
  {
    IRECT fillRect = r.FracRect(mDirection, GetValue(chIdx));
    
    g.FillRect(GetColor(kFG), fillRect); // TODO: shadows!
    
    IRECT peakRect;
    
    if(mDirection == kVertical)
      peakRect = IRECT(fillRect.L, fillRect.T, fillRect.R, fillRect.T + mPeakSize);
    else
      peakRect = IRECT(fillRect.R - mPeakSize, fillRect.T, fillRect.R, fillRect.B);
    
    DrawPeak(g, peakRect, chIdx);
  }
  
  virtual void DrawPeak(IGraphics& g, IRECT& r, int chIdx)
  {
    g.FillRect(GetColor(kHL), r);
  }
  
  void OnResize() override
  {
    MakeRects();
  }
  
protected:
  
  EDirection mDirection = EDirection::kVertical;
  WDL_TypedBuf<IRECT> mTrackBounds;
  float mMinTrackValue;
  float mMaxTrackValue;
  float mOuterPadding = 10.;
  float mTrackPadding = 2;
  float mPeakSize = 5.;
  bool mDrawTrackFrame = true;
};

/** A base class for buttons/momentary switches - cannot be linked to parameters.
 * The default action function triggers the default click function, which returns mValue to 0. after DEFAULT_ANIMATION_DURATION */
class IButtonControlBase : public IControl
{
public:
  IButtonControlBase(IRECT bounds, IActionFunction aF);
  
  virtual ~IButtonControlBase() {}
  void OnMouseDown(float x, float y, const IMouseMod& mod) override;
  void OnEndAnimation() override;
};

/** A base class for switch controls */
class ISwitchControlBase : public IControl
{
public:
  ISwitchControlBase(IRECT bounds, int paramIdx = kNoParameter, IActionFunction aF = nullptr, int numStates = 2);

  virtual ~ISwitchControlBase() {}
  void OnInit() override;
  void OnMouseDown(float x, float y, const IMouseMod& mod) override;
  void OnMouseUp(float x, float y, const IMouseMod& mod) override;
protected:
  int mNumStates;
  bool mMouseDown = false;
};

/** An abstract IControl base class that you can inherit from in order to make a control that pops up a menu to browse files */
class IDirBrowseControlBase : public IControl
{
public:
  IDirBrowseControlBase(IRECT bounds, const char* extension /* e.g. ".txt"*/)
  : IControl(bounds)
  {
    mExtension.Set(extension);
  }

  virtual ~IDirBrowseControlBase();

  int NItems();

  void AddPath(const char* path, const char* label);

  void SetUpMenu();

//  void GetSelectedItemLabel(WDL_String& label);
//  void GetSelectedItemPath(WDL_String& path);

private:
  void ScanDirectory(const char* path, IPopupMenu& menuToAddTo);
  void CollectSortedItems(IPopupMenu* pMenu);
  
protected:
  bool mShowEmptySubmenus = false;
  bool mShowFileExtensions = true;
  int mSelectedIndex = -1;
  IPopupMenu* mSelectedMenu = nullptr;
  IPopupMenu mMainMenu;
  WDL_PtrList<WDL_String> mPaths;
  WDL_PtrList<WDL_String> mPathLabels;
  WDL_PtrList<WDL_String> mFiles;
  WDL_PtrList<IPopupMenu::Item> mItems; // ptr to item for each file
  WDL_String mExtension;
};

/**@}*/

#pragma mark - BASIC CONTROLS

/**
 * \addtogroup Controls
 * @{
 */

/** A basic control to fill a rectangle with a color or gradient */
class IPanelControl : public IControl
{
public:
  IPanelControl(IRECT bounds, const IColor& color, bool drawFrame = false)
  : IControl(bounds, kNoParameter)
  , mPattern(color)
  , mDrawFrame(drawFrame)
  {
    mIgnoreMouse = true;
  }
  
  IPanelControl(IRECT bounds, const IPattern& pattern, bool drawFrame = false)
  : IControl(bounds, kNoParameter)
  , mPattern(pattern)
  , mDrawFrame(drawFrame)
  {
    mIgnoreMouse = true;
  }

  void Draw(IGraphics& g) override
  {
    if(g.HasPathSupport())
    {
      g.PathRect(mRECT);
      g.PathFill(mPattern);
    }
    else
      g.FillRect(mPattern.GetStop(0).mColor, mRECT);
    
    if(mDrawFrame)
      g.DrawRect(COLOR_LIGHT_GRAY, mRECT);
  }
  
  void SetPattern(const IPattern& pattern)
  {
    mPattern = pattern;
    SetDirty(false);
  }
  
private:
  IPattern mPattern;
  bool mDrawFrame;
};

/** A control that can be specialised with a lambda function, for quick experiments without making a custom IControl */
class ILambdaControl : public IControl
{
public:
  ILambdaControl(IRECT bounds, ILambdaDrawFunction drawFunc, int animationDuration = DEFAULT_ANIMATION_DURATION,
    bool loopAnimation = false, bool startImmediately = false, int paramIdx = kNoParameter)
  : IControl(bounds, paramIdx, DefaultClickActionFunc)
  , mDrawFunc(drawFunc)
  , mLoopAnimation(loopAnimation)
  , mAnimationDuration(animationDuration)
  {
    if (startImmediately)
    {
      SetAnimation(DefaultAnimationFunc);
      StartAnimation(mAnimationDuration);
    }
  }
  
  void Draw(IGraphics& g) override
  {
    if(mDrawFunc)
      mDrawFunc(this, g, mRECT);
  }
  
  virtual void OnEndAnimation() override // if you override this you must call the base implementation, to free mAnimationFunc
  {
    if(mLoopAnimation && mAnimationDuration)
      StartAnimation(mAnimationDuration);
    else
      SetAnimation(nullptr);
    
    SetDirty(false);
  }
  
  void OnMouseDown(float x, float y, const IMouseMod& mod) override
  {
    mMouseInfo.x = x; mMouseInfo.y = y; mMouseInfo.ms = mod;
    SetAnimation(DefaultAnimationFunc);
    StartAnimation(mAnimationDuration);
  }
  
  void OnMouseUp(float x, float y, const IMouseMod& mod) override { mMouseInfo.x = x; mMouseInfo.y = y; mMouseInfo.ms = mod; }
  void OnMouseDrag(float x, float y, float dX, float dY, const IMouseMod& mod) override { mMouseInfo.x = x; mMouseInfo.y = y; mMouseInfo.ms = mod; }
  void OnMouseDblClick(float x, float y, const IMouseMod& mod) override { mMouseInfo.x = x; mMouseInfo.y = y; mMouseInfo.ms = mod; }
  
  IMouseInfo GetMouseInfo() const { return mMouseInfo; }
//  ILayerPtr GetLayer() const { return mLayer; }

private:
  ILayerPtr mLayer;
  ILambdaDrawFunction mDrawFunc = nullptr;
  IMouseInfo mMouseInfo;
  bool mLoopAnimation;
  int mAnimationDuration;
};

/** A basic control to draw a bitmap, or one frame of a stacked bitmap depending on the current value. */
class IBitmapControl : public IControl
                     , public IBitmapBase
{
public:
  /** Creates a bitmap control
   * @param paramIdx Parameter index (-1 or kNoParameter, if this should not be linked to a parameter)
   * @param bitmap Image to be drawn */
  IBitmapControl(float x, float y, const IBitmap& bitmap, int paramIdx = kNoParameter, EBlendType blend = kBlendDefault)
  : IControl(IRECT(x, y, bitmap), paramIdx)
  , IBitmapBase(bitmap, blend)
  {}
  
  IBitmapControl(IRECT bounds, const IBitmap& bitmap, int paramIdx = kNoParameter, EBlendType blend = kBlendDefault)
  : IControl(bounds, paramIdx)
  , IBitmapBase(bitmap, blend)
  {}
  
  virtual ~IBitmapControl() {}

  void Draw(IGraphics& g) override;

  /** Implement to do something when graphics is scaled globally (e.g. moves to high DPI screen),
   *  if you override this make sure you call the parent method in order to rescale mBitmap */
  void OnRescale() override;
  
  void GrayOut(bool gray) override
  {
    IBitmapBase::GrayOut(gray);
    IControl::GrayOut(gray);
  }
};

/** A basic control to draw an SVG image to the screen. Optionally, cache SVG to an ILayer. */
class ISVGControl : public IControl
{
public:
  ISVGControl(IRECT bounds, ISVG& svg, bool useLayer = false)
    : IControl(bounds)
    , mSVG(svg)
    , mUseLayer(useLayer)
  {}

  virtual ~ISVGControl() {}

  void Draw(IGraphics& g) override
  {
    if(mUseLayer)
    {
      if (!g.CheckLayer(mLayer))
      {
        g.StartLayer(mRECT);
        g.DrawSVG(mSVG, mRECT);
        mLayer = g.EndLayer();
      }

      g.DrawLayer(mLayer);
    }
    else
      g.DrawSVG(mSVG, mRECT);
  }
  
  void SetSVG(const ISVG& svg)
  {
    mSVG = svg;
  }
  
private:
  bool mUseLayer;
  ILayerPtr mLayer;
  ISVG mSVG;
};

/** A basic control to display some text */
class ITextControl : public IControl
{
public:
  ITextControl(IRECT bounds, const char* str = "", const IText& text = DEFAULT_TEXT, const IColor& BGColor = DEFAULT_BGCOLOR)
  : IControl(bounds)
  , mStr(str)
  , mBGColor(BGColor)
  {
    mIgnoreMouse = true;
    IControl::mText = text;
  }

  void Draw(IGraphics& g) override;

  virtual void SetStr(const char* str);
  virtual void SetStrFmt(int maxlen, const char* fmt, ...);
  virtual void ClearStr() { SetStr(""); }
  
protected:
  WDL_String mStr;
  IColor mBGColor;
};

/** A control to display the textual representation of a parameter */
class ICaptionControl : public ITextControl
{
public:
  ICaptionControl(IRECT bounds, int paramIdx, const IText& text = DEFAULT_TEXT, bool showParamLabel = true);
  void Draw(IGraphics& g) override;
  void OnMouseDown(float x, float y, const IMouseMod& mod) override;
  void OnResize() override;
protected:
  bool mShowParamLabel;
  IRECT mTri;
};

/**@}*/

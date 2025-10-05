# LVGL Tab Design Guide for ESP32-S3 Touch LCD

## Overview
This guide explains how to add custom tabs to the LVGL-based UI on the ESP32-S3 Touch LCD 1.85" circular display (360x360). The UI uses LVGL v8.3.0 with a tabview system that allows users to swipe between different screens.

## Current Architecture

### Tab Structure
The application currently has 4 tabs:
1. **Tab 0** - Empty spacer tab (labeled "       ")
2. **Tab 1** - "Onboard" - System information panel
3. **Tab 2** - "Music" - Audio player interface  
4. **Tab 3** - Empty spacer tab (labeled "       ")

The empty tabs (0 and 3) act as visual padding and are automatically skipped by the `auto_switch()` timer.

### Key Files
- `LVGL_Arduino.ino` - Main sketch, initializes all components
- `LVGL_Example.cpp` - Tab view creation and tab content functions
- `LVGL_Example.h` - Function declarations
- `LVGL_Music.cpp` - Music player implementation (complex example)
- `LVGL_Driver.cpp` - LVGL initialization and loop handler

---

## How to Add a New Tab

### Step 1: Define Your Tab Creation Function

Add a static function declaration in `LVGL_Example.cpp` at the top with other prototypes:

```cpp
static void MyApp_create(lv_obj_t * parent);
```

### Step 2: Add the Tab to the Tabview

In the `Lvgl_Example1()` function around line 135-143, add your tab:

**Before:**
```cpp
lv_obj_t * t0 = lv_tabview_add_tab(tv, "       ");
lv_obj_t * t1 = lv_tabview_add_tab(tv, "Onboard");
lv_obj_t * t2 = lv_tabview_add_tab(tv, "music");
lv_obj_t * t3 = lv_tabview_add_tab(tv, "       ");

// Redirect_create1(t0);
Onboard_create(t1);
Music_create(t2);
// Redirect_create2(t3);
```

**After (adding "MyApp" tab):**
```cpp
lv_obj_t * t0 = lv_tabview_add_tab(tv, "       ");
lv_obj_t * t1 = lv_tabview_add_tab(tv, "Onboard");
lv_obj_t * t2 = lv_tabview_add_tab(tv, "music");
lv_obj_t * t3 = lv_tabview_add_tab(tv, "MyApp");      // Your new tab
lv_obj_t * t4 = lv_tabview_add_tab(tv, "       ");

// Redirect_create1(t0);
Onboard_create(t1);
Music_create(t2);
MyApp_create(t3);     // Initialize your tab content
// Redirect_create2(t4);
```

### Step 3: Implement the Tab Creation Function

Add the implementation at the end of `LVGL_Example.cpp` before the closing:

```cpp
static void MyApp_create(lv_obj_t * parent)
{
    // Create a panel (container)
    lv_obj_t * panel = lv_obj_create(parent);
    lv_obj_set_height(panel, LV_SIZE_CONTENT);
    
    // Add a title
    lv_obj_t * title = lv_label_create(panel);
    lv_label_set_text(title, "My Application");
    lv_obj_add_style(title, &style_title, 0);
    
    // Add your widgets here
    lv_obj_t * label = lv_label_create(panel);
    lv_label_set_text(label, "Hello World!");
    lv_obj_add_style(label, &style_text_muted, 0);
    
    // Set up grid layout (optional but recommended)
    static lv_coord_t grid_col_dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_row_dsc[] = {
        LV_GRID_CONTENT,  // Title
        LV_GRID_CONTENT,  // Label
        LV_GRID_TEMPLATE_LAST
    };
    lv_obj_set_grid_dsc_array(panel, grid_col_dsc, grid_row_dsc);
}
```

### Step 4: Update Auto-Switch Logic (Optional)

If you want the automatic tab switching to include your tab, modify the `auto_switch()` function:

```cpp
void IRAM_ATTR auto_switch(lv_timer_t * t)
{
  uint16_t page = lv_tabview_get_tab_act(tv);

  if (page == 0) { 
    lv_tabview_set_act(tv, 1, LV_ANIM_ON);  // Skip to Onboard
  } else if (page == 3) {                    // Your new tab
    lv_tabview_set_act(tv, 2, LV_ANIM_ON);  // Go back to Music
  } else if (page == 4) {                    // End spacer
    lv_tabview_set_act(tv, 1, LV_ANIM_ON);  // Loop back to start
  }
}
```

---

## LVGL Widget Guide

### Common Widgets Used in Tabs

#### 1. Labels (Static Text)
Labels are the most basic widget for displaying text.

**Basic Label:**
```cpp
lv_obj_t * label = lv_label_create(parent);
lv_label_set_text(label, "Your Text");
lv_obj_add_style(label, &style_text_muted, 0);  // Apply muted text style
```

**Dynamic Text with Formatting:**
```cpp
lv_obj_t * label = lv_label_create(parent);
lv_label_set_text_fmt(label, "Value: %d", 42);  // Printf-style formatting
```

**Label with Word Wrap:**
```cpp
lv_obj_t * label = lv_label_create(parent);
lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);  // Wrap long text
lv_obj_set_width(label, 150);  // Set width to enable wrapping
lv_label_set_text(label, "This is a long text that will wrap to multiple lines");
```

**Label with Text Alignment:**
```cpp
lv_obj_t * label = lv_label_create(parent);
lv_label_set_text(label, "Centered Text");
lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
```

**Label with Color Codes (Re-coloring):**
```cpp
lv_obj_t * label = lv_label_create(parent);
lv_label_set_recolor(label, true);  // Enable re-coloring
lv_label_set_text(label, "#ff0000 Red# #00ff00 Green# #0000ff Blue#");
```

**Scrolling Label:**
```cpp
lv_obj_t * label = lv_label_create(parent);
lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);  // Circular scroll
lv_obj_set_width(label, 150);
lv_label_set_text(label, "This text will scroll continuously. ");
```

**Label with Custom Font:**
```cpp
lv_obj_t * label = lv_label_create(parent);
lv_label_set_text(label, "Large Text");
lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
```

**Label Positioning:**
```cpp
lv_obj_t * label = lv_label_create(parent);
lv_label_set_text(label, "Centered");
lv_obj_center(label);  // Center in parent

lv_obj_align(label, LV_ALIGN_TOP_LEFT, 10, 10);  // Position relative to parent
lv_obj_align_to(label, other_widget, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);  // Relative to another widget
```

#### 2. Text Areas (Editable/Display Fields)
Text areas can be used as input fields or display-only fields.

**Single-Line Display Field (Read-Only):**
```cpp
lv_obj_t * textarea = lv_textarea_create(parent);
lv_textarea_set_one_line(textarea, true);
lv_textarea_set_placeholder_text(textarea, "Placeholder");
lv_textarea_set_text(textarea, "Content");  // Update content later
lv_obj_add_event_cb(textarea, ta_event_cb, LV_EVENT_ALL, NULL);
```

**Multi-Line Text Area:**
```cpp
lv_obj_t * textarea = lv_textarea_create(parent);
lv_obj_set_size(textarea, 200, 100);
lv_textarea_set_text(textarea, "Line 1\nLine 2\nLine 3");
```

**Editable Text Input:**
```cpp
lv_obj_t * textarea = lv_textarea_create(parent);
lv_textarea_set_one_line(textarea, true);
lv_obj_add_state(textarea, LV_STATE_FOCUSED);  // Show cursor
lv_obj_add_event_cb(textarea, textarea_event_cb, LV_EVENT_READY, NULL);

// Get text from textarea
const char * text = lv_textarea_get_text(textarea);
```

#### 3. Buttons
Buttons can be simple click targets or toggle buttons.

**Basic Button:**
```cpp
lv_obj_t * btn = lv_btn_create(parent);
lv_obj_set_size(btn, 120, 50);
lv_obj_t * btn_label = lv_label_create(btn);
lv_label_set_text(btn_label, "Click Me");
lv_obj_center(btn_label);
lv_obj_add_event_cb(btn, button_event_cb, LV_EVENT_CLICKED, NULL);
```

**Toggle Button:**
```cpp
lv_obj_t * btn = lv_btn_create(parent);
lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);  // Make it toggleable
lv_obj_set_height(btn, LV_SIZE_CONTENT);
lv_obj_t * label = lv_label_create(btn);
lv_label_set_text(label, "Toggle");
lv_obj_center(label);
lv_obj_add_event_cb(btn, button_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
```

**Floating Button (Always on Top):**
```cpp
lv_obj_t * btn = lv_btn_create(parent);
lv_obj_add_flag(btn, LV_OBJ_FLAG_FLOATING | LV_OBJ_FLAG_CLICKABLE);
lv_obj_align(btn, LV_ALIGN_BOTTOM_RIGHT, -15, -15);
lv_obj_set_style_radius(btn, LV_RADIUS_CIRCLE, 0);  // Circular button
lv_obj_set_size(btn, 50, 50);
```

**Button with Counter Example:**
```cpp
static void btn_event_cb(lv_event_t * e)
{
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        static uint8_t cnt = 0;
        cnt++;
        
        lv_obj_t * btn = lv_event_get_target(e);
        lv_obj_t * label = lv_obj_get_child(btn, 0);  // Get button's label
        lv_label_set_text_fmt(label, "Count: %d", cnt);
    }
}
```

#### 4. Sliders
Sliders for numeric input/adjustment.

**Basic Slider:**
```cpp
lv_obj_t * slider = lv_slider_create(parent);
lv_obj_set_size(slider, 200, 35);
lv_slider_set_range(slider, 0, 100);
lv_slider_set_value(slider, 50, LV_ANIM_ON);
lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
```

**Slider with Value Display:**
```cpp
static lv_obj_t * slider_label;

static void slider_event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target(e);
    int value = lv_slider_get_value(slider);
    
    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d%%", value);
    lv_label_set_text(slider_label, buf);
}

// In create function:
lv_obj_t * slider = lv_slider_create(parent);
lv_obj_center(slider);
lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

slider_label = lv_label_create(parent);
lv_label_set_text(slider_label, "0%");
lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
```

**Customized Slider (from Onboard example):**
```cpp
lv_obj_t * slider = lv_slider_create(parent);
lv_obj_add_flag(slider, LV_OBJ_FLAG_CLICKABLE);
lv_obj_set_size(slider, 200, 35);
lv_obj_set_style_radius(slider, 3, LV_PART_KNOB);
lv_obj_set_style_bg_opa(slider, LV_OPA_TRANSP, LV_PART_KNOB);
lv_obj_set_style_bg_color(slider, lv_color_hex(0xAAAAAA), LV_PART_KNOB);
lv_obj_set_style_bg_color(slider, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR);
lv_obj_set_style_outline_width(slider, 2, LV_PART_INDICATOR);
lv_obj_set_style_outline_color(slider, lv_color_hex(0xD3D3D3), LV_PART_INDICATOR);
lv_slider_set_range(slider, 5, 100);
lv_slider_set_value(slider, 50, LV_ANIM_ON);
```

#### 5. Switches
Toggle switches for on/off states.

```cpp
lv_obj_t * sw = lv_switch_create(parent);
lv_obj_add_event_cb(sw, switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

// Check state
if(lv_obj_has_state(sw, LV_STATE_CHECKED)) {
    // Switch is ON
} else {
    // Switch is OFF
}
```

#### 6. Checkboxes
Checkboxes with integrated labels.

```cpp
lv_obj_t * cb = lv_checkbox_create(parent);
lv_checkbox_set_text(cb, "Enable Feature");
lv_obj_add_event_cb(cb, checkbox_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
```

#### 7. Arcs (Circular Progress/Input)
Perfect for circular displays like this ESP32-S3.

```cpp
lv_obj_t * arc = lv_arc_create(parent);
lv_arc_set_range(arc, 0, 100);
lv_arc_set_value(arc, 50);
lv_arc_set_rotation(arc, 135);
lv_arc_set_bg_angles(arc, 0, 270);
lv_obj_center(arc);
```

#### 8. Dropdown Menus
```cpp
lv_obj_t * dropdown = lv_dropdown_create(parent);
lv_dropdown_set_options(dropdown, "Option 1\nOption 2\nOption 3");
lv_obj_add_event_cb(dropdown, dropdown_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

// Get selected option
uint16_t selected = lv_dropdown_get_selected(dropdown);
```

#### 9. Button Matrix (Keyboard-like Grid)
Useful for creating custom keyboards or control panels.

```cpp
static const char * btnm_map[] = {
    "1", "2", "3", "\n",
    "4", "5", "6", "\n",
    "7", "8", "9", "\n",
    "Clear", "0", "OK", ""
};

lv_obj_t * btnm = lv_btnmatrix_create(parent);
lv_obj_set_size(btnm, 200, 150);
lv_btnmatrix_set_map(btnm, btnm_map);
lv_obj_add_event_cb(btnm, btnm_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

// In event handler:
const char * txt = lv_btnmatrix_get_btn_text(obj, lv_btnmatrix_get_selected_btn(obj));
```

---

## Working with Global Variables

### Declaring Widget Pointers for Updates

If you need to update a widget from outside your tab creation function (e.g., from a timer or sensor callback):

**In `LVGL_Example.cpp` at the top with other globals (~line 55):**
```cpp
lv_obj_t * MyApp_Value_Display;
lv_obj_t * MyApp_Status_Label;
```

**Use in your create function:**
```cpp
static void MyApp_create(lv_obj_t * parent)
{
    // ... other code ...
    
    MyApp_Value_Display = lv_textarea_create(panel);
    lv_textarea_set_one_line(MyApp_Value_Display, true);
    lv_textarea_set_placeholder_text(MyApp_Value_Display, "Value");
}
```

**Update from anywhere:**
```cpp
void Update_MyApp_Display(const char* text) {
    if (MyApp_Value_Display != NULL) {
        lv_textarea_set_text(MyApp_Value_Display, text);
    }
}
```

---

## Object Positioning and Alignment

### Alignment Options

LVGL provides flexible alignment options for positioning widgets.

**Align to Parent:**
```cpp
lv_obj_align(obj, LV_ALIGN_CENTER, x_offset, y_offset);
```

**Available Alignments:**
- `LV_ALIGN_CENTER` - Center of parent
- `LV_ALIGN_TOP_LEFT`, `LV_ALIGN_TOP_MID`, `LV_ALIGN_TOP_RIGHT`
- `LV_ALIGN_LEFT_MID`, `LV_ALIGN_RIGHT_MID`
- `LV_ALIGN_BOTTOM_LEFT`, `LV_ALIGN_BOTTOM_MID`, `LV_ALIGN_BOTTOM_RIGHT`

**Align Relative to Another Object:**
```cpp
lv_obj_align_to(obj, reference_obj, LV_ALIGN_OUT_BOTTOM_MID, x_offset, y_offset);
```

**Available Relative Alignments:**
- `LV_ALIGN_OUT_TOP_LEFT`, `LV_ALIGN_OUT_TOP_MID`, `LV_ALIGN_OUT_TOP_RIGHT`
- `LV_ALIGN_OUT_BOTTOM_LEFT`, `LV_ALIGN_OUT_BOTTOM_MID`, `LV_ALIGN_OUT_BOTTOM_RIGHT`
- `LV_ALIGN_OUT_LEFT_TOP`, `LV_ALIGN_OUT_LEFT_MID`, `LV_ALIGN_OUT_LEFT_BOTTOM`
- `LV_ALIGN_OUT_RIGHT_TOP`, `LV_ALIGN_OUT_RIGHT_MID`, `LV_ALIGN_OUT_RIGHT_BOTTOM`

**Set Position and Size:**
```cpp
lv_obj_set_pos(obj, x, y);           // Set position
lv_obj_set_x(obj, x);                // Set X only
lv_obj_set_y(obj, y);                // Set Y only
lv_obj_set_size(obj, width, height); // Set size
lv_obj_set_width(obj, width);        // Set width only
lv_obj_set_height(obj, height);      // Set height only
```

**Special Size Values:**
```cpp
lv_obj_set_height(obj, LV_SIZE_CONTENT);  // Size to content
lv_obj_set_width(obj, LV_PCT(50));        // 50% of parent width
```

---

## Grid Layout Advanced

### Positioning Objects in Grid Cells

After setting up a grid, assign widgets to specific cells:

```cpp
lv_obj_set_grid_cell(obj, 
    LV_GRID_ALIGN_START,  // Column alignment
    0,                     // Column position
    2,                     // Column span
    LV_GRID_ALIGN_CENTER,  // Row alignment
    1,                     // Row position
    1);                    // Row span
```

**Grid Cell Parameters:**
1. Column alignment: `LV_GRID_ALIGN_START`, `LV_GRID_ALIGN_CENTER`, `LV_GRID_ALIGN_END`, `LV_GRID_ALIGN_STRETCH`
2. Column position: 0-based index
3. Column span: number of columns to occupy
4. Row alignment: same options as column alignment
5. Row position: 0-based index
6. Row span: number of rows to occupy

**Example - Label Spanning Multiple Columns:**
```cpp
static lv_coord_t grid_col_dsc[] = {100, 100, 100, LV_GRID_TEMPLATE_LAST};
static lv_coord_t grid_row_dsc[] = {50, 50, LV_GRID_TEMPLATE_LAST};
lv_obj_set_grid_dsc_array(panel, grid_col_dsc, grid_row_dsc);

lv_obj_t * title = lv_label_create(panel);
lv_label_set_text(title, "Wide Title");
lv_obj_set_grid_cell(title, LV_GRID_ALIGN_CENTER, 0, 3, LV_GRID_ALIGN_CENTER, 0, 1);
// Spans all 3 columns in row 0
```

---

## Animations

### Using Animations

Animations make the UI feel responsive and professional.

**Simple Animation Example (from color_changer):**
```cpp
static void animate_width(lv_obj_t * obj, int32_t start_val, int32_t end_val)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_exec_cb(&a, anim_width_cb);  // Callback to execute
    lv_anim_set_values(&a, start_val, end_val);
    lv_anim_set_time(&a, 200);  // Duration in ms
    lv_anim_start(&a);
}

// Animation callback - called for each frame
static void anim_width_cb(void * var, int32_t v)
{
    lv_obj_set_width((lv_obj_t *)var, v);
}
```

**Built-in Animation Easings:**
- `lv_anim_path_linear` - Constant speed
- `lv_anim_path_ease_in` - Slow start
- `lv_anim_path_ease_out` - Slow end
- `lv_anim_path_ease_in_out` - Slow start and end
- `lv_anim_path_bounce` - Bounce effect

**Animation with Easing:**
```cpp
lv_anim_t a;
lv_anim_init(&a);
lv_anim_set_var(&a, obj);
lv_anim_set_exec_cb(&a, anim_cb);
lv_anim_set_values(&a, 0, 100);
lv_anim_set_time(&a, 500);
lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
lv_anim_start(&a);
```

**Delete All Animations:**
```cpp
lv_anim_del(NULL, NULL);  // Delete all animations
lv_anim_del(obj, NULL);   // Delete animations for specific object
```

---

## Drawing and Canvas

### Canvas Widget for Custom Graphics

For custom graphics, use the canvas widget:

```cpp
#define CANVAS_WIDTH  200
#define CANVAS_HEIGHT 150

// Buffer for canvas (must be static or global)
static lv_color_t canvas_buffer[CANVAS_WIDTH * CANVAS_HEIGHT];

lv_obj_t * canvas = lv_canvas_create(parent);
lv_canvas_set_buffer(canvas, canvas_buffer, CANVAS_WIDTH, CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR);
lv_obj_center(canvas);

// Draw on canvas
lv_canvas_fill_bg(canvas, lv_color_white(), LV_OPA_COVER);
lv_canvas_draw_rect(canvas, 10, 10, 50, 30, lv_color_black());
lv_canvas_draw_line(canvas, 0, 0, 100, 100, lv_color_red());
lv_canvas_draw_text(canvas, 20, 20, 100, "Hello", lv_color_blue());
```

**Canvas Drawing Functions:**
- `lv_canvas_fill_bg()` - Fill background
- `lv_canvas_draw_rect()` - Draw rectangle
- `lv_canvas_draw_line()` - Draw line
- `lv_canvas_draw_arc()` - Draw arc
- `lv_canvas_draw_polygon()` - Draw polygon
- `lv_canvas_draw_text()` - Draw text

---

## Object Flags and States

### Object Flags

Control object behavior with flags:

```cpp
lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);    // Make clickable
lv_obj_add_flag(obj, LV_OBJ_FLAG_FLOATING);     // Float above other objects
lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);    // Toggle state
lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);       // Hide object

lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);  // Remove flag
```

**Common Flags:**
- `LV_OBJ_FLAG_CLICKABLE` - Can be clicked
- `LV_OBJ_FLAG_CHECKABLE` - Toggle button behavior
- `LV_OBJ_FLAG_HIDDEN` - Hidden from view
- `LV_OBJ_FLAG_FLOATING` - Stays on top
- `LV_OBJ_FLAG_SCROLLABLE` - Can scroll content
- `LV_OBJ_FLAG_SCROLL_ON_FOCUS` - Auto-scroll when focused
- `LV_OBJ_FLAG_CLICK_FOCUSABLE` - Can gain focus on click

### Object States

Objects can have different visual states:

```cpp
lv_obj_add_state(obj, LV_STATE_CHECKED);    // Add checked state
lv_obj_clear_state(obj, LV_STATE_CHECKED);  // Remove state

if(lv_obj_has_state(obj, LV_STATE_CHECKED)) {
    // Object is checked
}
```

**Common States:**
- `LV_STATE_DEFAULT` - Normal state
- `LV_STATE_CHECKED` - Toggled/selected
- `LV_STATE_FOCUSED` - Has focus (touch/cursor)
- `LV_STATE_PRESSED` - Being pressed
- `LV_STATE_DISABLED` - Disabled/grayed out

---

## Event Handling

### Creating Event Callbacks

Event callbacks let your widgets respond to user interactions:

```cpp
static void my_button_event_cb(lv_event_t * e) 
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    
    if (code == LV_EVENT_CLICKED) {
        Serial.println("Button clicked!");
        // Do something...
    }
}
```

### Common Event Types

- `LV_EVENT_CLICKED` - Button/object clicked
- `LV_EVENT_VALUE_CHANGED` - Slider, switch, or arc value changed
- `LV_EVENT_FOCUSED` - Widget gained focus (touch)
- `LV_EVENT_DEFOCUSED` - Widget lost focus
- `LV_EVENT_PRESSED` - Object pressed down
- `LV_EVENT_RELEASED` - Object released
- `LV_EVENT_READY` - Input finished (e.g., Enter pressed in textarea)
- `LV_EVENT_ALL` - Listen to all events

### Event Data and User Data

**Passing Custom Data to Event Handler:**
```cpp
// Pass pointer to data as user_data
int my_value = 42;
lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, &my_value);

// Retrieve in callback
static void btn_event_cb(lv_event_t * e)
{
    int * value = (int *)lv_event_get_user_data(e);
    Serial.printf("User data value: %d\n", *value);
}
```

**Getting Information from Event:**
```cpp
static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);      // What happened
    lv_obj_t * obj = lv_event_get_target(e);           // Widget that triggered event
    lv_obj_t * current = lv_event_get_current_target(e); // Current handler target
    void * user_data = lv_event_get_user_data(e);     // Custom data
}
```

**Getting Widget Values:**
```cpp
// Slider
int value = lv_slider_get_value(slider);

// Switch/Checkbox
bool is_checked = lv_obj_has_state(obj, LV_STATE_CHECKED);

// Textarea
const char * text = lv_textarea_get_text(textarea);

// Dropdown
uint16_t selected = lv_dropdown_get_selected(dropdown);
char buf[32];
lv_dropdown_get_selected_str(dropdown, buf, sizeof(buf));

// Button matrix
uint16_t btn_id = lv_btnmatrix_get_selected_btn(btnm);
const char * btn_text = lv_btnmatrix_get_btn_text(btnm, btn_id);
```

**Accessing Child Widgets:**
```cpp
// Get first child (e.g., label inside button)
lv_obj_t * child = lv_obj_get_child(parent, 0);

// Get parent
lv_obj_t * parent = lv_obj_get_parent(obj);
```

---

## Layout Systems

### Grid Layout (Recommended for Panels)

Grid layout automatically arranges widgets in rows and columns:

```cpp
// Define column widths
static lv_coord_t grid_col_dsc[] = {
    LV_GRID_CONTENT,  // Size to content
    LV_GRID_FR(1),    // Take remaining space (flex)
    LV_GRID_TEMPLATE_LAST
};

// Define row heights
static lv_coord_t grid_row_dsc[] = {
    LV_GRID_CONTENT,  // Row 1: size to content
    40,               // Row 2: fixed 40px
    LV_GRID_CONTENT,  // Row 3: size to content
    LV_GRID_TEMPLATE_LAST
};

lv_obj_set_grid_dsc_array(panel, grid_col_dsc, grid_row_dsc);
```

### Flex Layout

For simple horizontal/vertical arrangements:

```cpp
lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW);  // or LV_FLEX_FLOW_COLUMN
lv_obj_set_flex_align(parent, 
    LV_FLEX_ALIGN_SPACE_EVENLY,  // Main axis
    LV_FLEX_ALIGN_CENTER,         // Cross axis
    LV_FLEX_ALIGN_CENTER);        // Tracks
```

---

## Styling

### Available Global Styles

Defined in `Lvgl_Example1()`:
- `style_text_muted` - 90% opacity text (for labels)
- `style_title` - Large font for titles
- `style_icon` - Primary color large font for icons
- `style_bullet` - Circular bullet points

### Using Styles

```cpp
lv_obj_add_style(my_label, &style_text_muted, 0);
```

### Custom Styling

```cpp
static lv_style_t my_custom_style;
lv_style_init(&my_custom_style);
lv_style_set_bg_color(&my_custom_style, lv_color_hex(0xFF0000));
lv_style_set_radius(&my_custom_style, 10);
lv_obj_add_style(my_obj, &my_custom_style, 0);
```

### Color Helpers

```cpp
lv_color_hex(0xRRGGBB)           // Hex color
lv_palette_main(LV_PALETTE_RED)  // Named palette
lv_color_white()                  // Predefined colors
lv_color_black()
```

---

## Example: Simple Sensor Display Tab

Here's a complete example showing temperature and humidity:

```cpp
// Global variables for updating
lv_obj_t * Temp_Display;
lv_obj_t * Humid_Display;

static void Sensors_create(lv_obj_t * parent)
{
    // Create panel
    lv_obj_t * panel = lv_obj_create(parent);
    lv_obj_set_height(panel, LV_SIZE_CONTENT);
    
    // Title
    lv_obj_t * title = lv_label_create(panel);
    lv_label_set_text(title, "Environmental Sensors");
    lv_obj_add_style(title, &style_title, 0);
    
    // Temperature Label
    lv_obj_t * temp_label = lv_label_create(panel);
    lv_label_set_text(temp_label, "Temperature");
    lv_obj_add_style(temp_label, &style_text_muted, 0);
    
    // Temperature Display
    Temp_Display = lv_textarea_create(panel);
    lv_textarea_set_one_line(Temp_Display, true);
    lv_textarea_set_placeholder_text(Temp_Display, "-- °C");
    
    // Humidity Label
    lv_obj_t * humid_label = lv_label_create(panel);
    lv_label_set_text(humid_label, "Humidity");
    lv_obj_add_style(humid_label, &style_text_muted, 0);
    
    // Humidity Display
    Humid_Display = lv_textarea_create(panel);
    lv_textarea_set_one_line(Humid_Display, true);
    lv_textarea_set_placeholder_text(Humid_Display, "-- %");
    
    // Set up grid
    static lv_coord_t grid_col_dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_row_dsc[] = {
        LV_GRID_CONTENT,  // Title
        5,                // Separator
        LV_GRID_CONTENT,  // Temp label
        40,               // Temp display
        LV_GRID_CONTENT,  // Humid label
        40,               // Humid display
        LV_GRID_TEMPLATE_LAST
    };
    lv_obj_set_grid_dsc_array(panel, grid_col_dsc, grid_row_dsc);
}

// Update function (call from main loop or timer)
void Update_Sensor_Display(float temp, float humidity)
{
    char buf[20];
    
    if (Temp_Display != NULL) {
        snprintf(buf, sizeof(buf), "%.1f °C", temp);
        lv_textarea_set_text(Temp_Display, buf);
    }
    
    if (Humid_Display != NULL) {
        snprintf(buf, sizeof(buf), "%.1f %%", humidity);
        lv_textarea_set_text(Humid_Display, buf);
    }
}
```

Add to `LVGL_Example.h`:
```cpp
void Update_Sensor_Display(float temp, float humidity);
```

---

## Example: Interactive Control Tab

Tab with buttons and a slider:

```cpp
// Global for slider value tracking
lv_obj_t * Motor_Speed_Slider;
lv_obj_t * Motor_Status_Label;

static void motor_start_event_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        Serial.println("Motor START");
        lv_label_set_text(Motor_Status_Label, "Running");
        // Add your motor control code
    }
}

static void motor_stop_event_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        Serial.println("Motor STOP");
        lv_label_set_text(Motor_Status_Label, "Stopped");
        // Add your motor control code
    }
}

static void motor_speed_event_cb(lv_event_t * e)
{
    int speed = lv_slider_get_value(lv_event_get_target(e));
    Serial.printf("Motor speed: %d\n", speed);
    // Set motor PWM or similar
}

static void MotorControl_create(lv_obj_t * parent)
{
    lv_obj_t * panel = lv_obj_create(parent);
    lv_obj_set_height(panel, LV_SIZE_CONTENT);
    
    // Title
    lv_obj_t * title = lv_label_create(panel);
    lv_label_set_text(title, "Motor Control");
    lv_obj_add_style(title, &style_title, 0);
    
    // Status label
    lv_obj_t * status_label = lv_label_create(panel);
    lv_label_set_text(status_label, "Status:");
    lv_obj_add_style(status_label, &style_text_muted, 0);
    
    Motor_Status_Label = lv_label_create(panel);
    lv_label_set_text(Motor_Status_Label, "Stopped");
    
    // Start button
    lv_obj_t * start_btn = lv_btn_create(panel);
    lv_obj_set_size(start_btn, 120, 50);
    lv_obj_t * start_label = lv_label_create(start_btn);
    lv_label_set_text(start_label, "START");
    lv_obj_center(start_label);
    lv_obj_add_event_cb(start_btn, motor_start_event_cb, LV_EVENT_CLICKED, NULL);
    
    // Stop button
    lv_obj_t * stop_btn = lv_btn_create(panel);
    lv_obj_set_size(stop_btn, 120, 50);
    lv_obj_t * stop_label = lv_label_create(stop_btn);
    lv_label_set_text(stop_label, "STOP");
    lv_obj_center(stop_label);
    lv_obj_add_event_cb(stop_btn, motor_stop_event_cb, LV_EVENT_CLICKED, NULL);
    
    // Speed slider
    lv_obj_t * speed_label = lv_label_create(panel);
    lv_label_set_text(speed_label, "Speed");
    lv_obj_add_style(speed_label, &style_text_muted, 0);
    
    Motor_Speed_Slider = lv_slider_create(panel);
    lv_obj_set_size(Motor_Speed_Slider, 200, 35);
    lv_slider_set_range(Motor_Speed_Slider, 0, 255);
    lv_slider_set_value(Motor_Speed_Slider, 128, LV_ANIM_OFF);
    lv_obj_add_event_cb(Motor_Speed_Slider, motor_speed_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Grid layout
    static lv_coord_t grid_col_dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_row_dsc[] = {
        LV_GRID_CONTENT,  // Title
        5,                // Separator
        LV_GRID_CONTENT,  // Status label
        LV_GRID_CONTENT,  // Status value
        50,               // Start button
        50,               // Stop button
        LV_GRID_CONTENT,  // Speed label
        40,               // Speed slider
        LV_GRID_TEMPLATE_LAST
    };
    lv_obj_set_grid_dsc_array(panel, grid_col_dsc, grid_row_dsc);
}
```

---

## Timers for Periodic Updates

### LVGL Timers

LVGL timers let you update widgets periodically without blocking.

**Create a Timer:**
```cpp
// Timer function signature
void IRAM_ATTR my_timer_cb(lv_timer_t * timer)
{
    // Update your widgets here
    static int counter = 0;
    counter++;
    
    char buf[32];
    snprintf(buf, sizeof(buf), "Count: %d", counter);
    if (My_Label != NULL) {
        lv_label_set_text(My_Label, buf);
    }
}

// In setup or tab create function
lv_timer_t * my_timer = lv_timer_create(my_timer_cb, 1000, NULL);  // 1000ms = 1 second
```

**Timer with User Data:**
```cpp
typedef struct {
    lv_obj_t * label;
    int max_value;
} timer_data_t;

void IRAM_ATTR timer_cb(lv_timer_t * timer)
{
    timer_data_t * data = (timer_data_t *)timer->user_data;
    // Use data->label and data->max_value
}

// Create timer with data
static timer_data_t my_data = {.label = my_label, .max_value = 100};
lv_timer_t * timer = lv_timer_create(timer_cb, 500, &my_data);
```

**Timer Control:**
```cpp
lv_timer_pause(timer);      // Pause timer
lv_timer_resume(timer);     // Resume timer
lv_timer_del(timer);        // Delete timer
lv_timer_reset(timer);      // Reset timer period

// Change period
lv_timer_set_period(timer, 2000);  // Change to 2 seconds
```

**One-Shot Timer:**
```cpp
lv_timer_t * timer = lv_timer_create(my_callback, 3000, NULL);
lv_timer_set_repeat_count(timer, 1);  // Run only once
```

### Updating Widgets from Main Loop

You can also update widgets from your main Arduino loop or FreeRTOS tasks:

```cpp
// In LVGL_Arduino.ino or main loop
void loop() {
    // Your sensor reading
    float temperature = readSensor();
    
    // Update display (must be thread-safe)
    Update_Temperature_Display(temperature);
    
    delay(1000);
}

// In LVGL_Example.cpp
void Update_Temperature_Display(float temp) {
    if (Temp_Display != NULL) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%.1f °C", temp);
        lv_textarea_set_text(Temp_Display, buf);
    }
}
```

**Thread Safety Note:** When updating LVGL widgets from Arduino loop or other tasks, ensure you're calling LVGL functions within the LVGL task context or use appropriate locking mechanisms.

---

## Color and Opacity

### Color Formats

**Hex Colors:**
```cpp
lv_color_t color = lv_color_hex(0xFF5733);  // RGB hex
lv_color_t red = lv_color_hex(0xFF0000);
lv_color_t green = lv_color_hex(0x00FF00);
lv_color_t blue = lv_color_hex(0x0000FF);
```

**Named Colors:**
```cpp
lv_color_t white = lv_color_white();
lv_color_t black = lv_color_black();
```

**Palette Colors:**
```cpp
lv_color_t color = lv_palette_main(LV_PALETTE_RED);
lv_color_t lighter = lv_palette_lighten(LV_PALETTE_RED, 1);
lv_color_t darker = lv_palette_darken(LV_PALETTE_RED, 1);
```

**Available Palettes:**
`LV_PALETTE_RED`, `LV_PALETTE_PINK`, `LV_PALETTE_PURPLE`, `LV_PALETTE_DEEP_PURPLE`, `LV_PALETTE_INDIGO`, `LV_PALETTE_BLUE`, `LV_PALETTE_LIGHT_BLUE`, `LV_PALETTE_CYAN`, `LV_PALETTE_TEAL`, `LV_PALETTE_GREEN`, `LV_PALETTE_LIGHT_GREEN`, `LV_PALETTE_LIME`, `LV_PALETTE_YELLOW`, `LV_PALETTE_AMBER`, `LV_PALETTE_ORANGE`, `LV_PALETTE_DEEP_ORANGE`, `LV_PALETTE_BROWN`, `LV_PALETTE_GREY`, `LV_PALETTE_BLUE_GREY`

**RGB Components:**
```cpp
lv_color_t color = lv_color_make(255, 128, 64);  // R, G, B
```

### Opacity (Alpha)

Opacity values range from 0 (transparent) to 255 (opaque):

```cpp
lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);    // 100% opaque (255)
lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, 0);   // 0% opaque (0)
lv_obj_set_style_bg_opa(obj, LV_OPA_90, 0);       // 90% opaque
lv_obj_set_style_bg_opa(obj, LV_OPA_50, 0);       // 50% opaque
lv_obj_set_style_bg_opa(obj, 200, 0);             // Custom (0-255)
```

**Opacity Constants:**
- `LV_OPA_TRANSP` = 0
- `LV_OPA_10` to `LV_OPA_90` (10% increments)
- `LV_OPA_COVER` = 255

---

## Symbols and Icons

### Built-in Symbols

LVGL includes many built-in symbols that work with any font:

```cpp
lv_label_set_text(label, LV_SYMBOL_HOME " Home");
lv_label_set_text(label, LV_SYMBOL_SETTINGS " Settings");
lv_label_set_text(label, LV_SYMBOL_OK " Confirm");
```

**Commonly Used Symbols:**
- `LV_SYMBOL_AUDIO` - 🔊
- `LV_SYMBOL_VIDEO` - 🎬
- `LV_SYMBOL_LIST` - ☰
- `LV_SYMBOL_OK` - ✓
- `LV_SYMBOL_CLOSE` - ✕
- `LV_SYMBOL_POWER` - ⏻
- `LV_SYMBOL_SETTINGS` - ⚙
- `LV_SYMBOL_HOME` - 🏠
- `LV_SYMBOL_DOWNLOAD` - ⬇
- `LV_SYMBOL_UPLOAD` - ⬆
- `LV_SYMBOL_DRIVE` - 💾
- `LV_SYMBOL_REFRESH` - ⟳
- `LV_SYMBOL_MUTE` - 🔇
- `LV_SYMBOL_VOLUME_MID` - 🔉
- `LV_SYMBOL_VOLUME_MAX` - 🔊
- `LV_SYMBOL_IMAGE` - 🖼
- `LV_SYMBOL_EDIT` - ✎
- `LV_SYMBOL_PREV` - ◀
- `LV_SYMBOL_PLAY` - ▶
- `LV_SYMBOL_PAUSE` - ⏸
- `LV_SYMBOL_STOP` - ⏹
- `LV_SYMBOL_NEXT` - ▶▶
- `LV_SYMBOL_BATTERY_FULL` - 🔋
- `LV_SYMBOL_BATTERY_3` - 🔋 (75%)
- `LV_SYMBOL_BATTERY_2` - 🔋 (50%)
- `LV_SYMBOL_BATTERY_1` - 🔋 (25%)
- `LV_SYMBOL_BATTERY_EMPTY` - 🔋 (0%)
- `LV_SYMBOL_BLUETOOTH` - 
- `LV_SYMBOL_GPS` - 
- `LV_SYMBOL_WIFI` - 📶
- `LV_SYMBOL_WARNING` - ⚠
- `LV_SYMBOL_BELL` - 🔔
- `LV_SYMBOL_BACKSPACE` - ⌫
- `LV_SYMBOL_NEW_LINE` - ⏎

**Using in Buttons:**
```cpp
lv_obj_t * btn = lv_btn_create(parent);
lv_obj_set_style_bg_img_src(btn, LV_SYMBOL_SETTINGS, 0);
```

---

## Practical Tips

### Memory Management

**Static Variables for Grid Layouts:**
Always use `static` for grid descriptors to prevent stack issues:
```cpp
static lv_coord_t grid_col_dsc[] = {...};  // ✓ Correct
lv_coord_t grid_col_dsc[] = {...};          // ✗ Wrong - will crash!
```

**Buffer for Canvas:**
Canvas buffers must be static or global:
```cpp
static lv_color_t canvas_buf[WIDTH * HEIGHT];  // ✓ Correct
```

### Performance Optimization

**Minimize Redraws:**
```cpp
// Batch updates
lv_obj_invalidate(obj);  // Mark for redraw, but don't draw yet
// ... make multiple changes ...
lv_refr_now(NULL);  // Force redraw now
```

**Use Double Buffering:**
Already enabled in the driver configuration for smooth animations.

### Debugging

**Enable LVGL Logging:**
In `lv_conf.h`:
```c
#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_TRACE
```

**Print Widget Info:**
```cpp
Serial.printf("Widget width: %d, height: %d\n", 
    lv_obj_get_width(obj), lv_obj_get_height(obj));
Serial.printf("Widget x: %d, y: %d\n", 
    lv_obj_get_x(obj), lv_obj_get_y(obj));
```

### Touch Coordinates

**Get Touch Position:**
```cpp
static void touch_event_cb(lv_event_t * e)
{
    lv_indev_t * indev = lv_indev_get_act();
    lv_point_t point;
    lv_indev_get_point(indev, &point);
    Serial.printf("Touch at x:%d, y:%d\n", point.x, point.y);
}
```

### Text Formatting

**Safe String Formatting:**
```cpp
char buf[32];
lv_snprintf(buf, sizeof(buf), "Value: %d", value);  // Use lv_snprintf
lv_label_set_text(label, buf);
```

**Dynamic Text Updates:**
```cpp
lv_label_set_text_fmt(label, "Temp: %.1f°C", temperature);  // Direct formatting
```

---

## Display Specifications

- **Resolution**: 360x360 pixels
- **Shape**: Circular display
- **Touch**: Capacitive touch (CST816 controller)
- **Tab Height**: 45 pixels (defined in `Lvgl_Example1()`)
- **Content Area**: ~315 pixels vertical (after tab bar)

---

## Best Practices

1. **Use Grid Layout**: Provides consistent spacing and automatic arrangement
2. **Test on Hardware**: The circular display clips corners differently than simulators
3. **Keep It Simple**: Small display - avoid clutter
4. **Use Global Pointers**: For widgets that need updates from outside the tab
5. **Event Callbacks**: Keep them lightweight and fast
6. **Static Variables**: Use static for grid descriptors to avoid stack issues
7. **NULL Checks**: Always check widget pointers before updating
8. **Consistent Styling**: Use the predefined global styles for consistency

---

## Common Pitfalls

❌ **Don't**: Create widgets without a parent
```cpp
lv_obj_t * label = lv_label_create(NULL);  // Wrong!
```

✅ **Do**: Always specify the parent (panel or tab)
```cpp
lv_obj_t * label = lv_label_create(panel);  // Correct
```

❌ **Don't**: Forget to declare grid arrays as static
```cpp
lv_coord_t grid_col_dsc[] = {...};  // Will cause crashes!
```

✅ **Do**: Use static keyword
```cpp
static lv_coord_t grid_col_dsc[] = {...};  // Correct
```

❌ **Don't**: Update widgets without NULL checks
```cpp
lv_textarea_set_text(My_Display, "text");  // Crashes if NULL
```

✅ **Do**: Check pointer first
```cpp
if (My_Display != NULL) {
    lv_textarea_set_text(My_Display, "text");
}
```

---

## Flash Storage (Preferences Library)

The ESP32-S3 has built-in Non-Volatile Storage (NVS) that persists data across reboots. Use the `Preferences` library for key-value storage.

### Setup Requirements

**Important**: Must initialize NVS before using Preferences!

```cpp
#include <Preferences.h>
#include <nvs_flash.h>

Preferences preferences;

void setup() {
    // Initialize NVS flash (REQUIRED!)
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        err = nvs_flash_init();
    }
    
    // Now safe to use Preferences
}
```

### Basic Read/Write Operations

**Writing Data:**
```cpp
void saveSettings() {
    if (preferences.begin("settings", false)) {  // false = read-write mode
        preferences.putInt("brightness", 80);
        preferences.putString("device_name", "MyESP32");
        preferences.putBool("wifi_enabled", true);
        preferences.putFloat("volume", 0.75);
        preferences.end();
        Serial.println("Settings saved!");
    } else {
        Serial.println("Failed to open Preferences");
    }
}
```

**Reading Data:**
```cpp
void loadSettings() {
    if (preferences.begin("settings", true)) {  // true = read-only mode
        int brightness = preferences.getInt("brightness", 50);      // 50 = default
        String name = preferences.getString("device_name", "ESP32"); // "ESP32" = default
        bool wifi = preferences.getBool("wifi_enabled", true);       // true = default
        float volume = preferences.getFloat("volume", 0.5);         // 0.5 = default
        preferences.end();
        
        Serial.printf("Brightness: %d\n", brightness);
        Serial.printf("Name: %s\n", name.c_str());
    }
}
```

### Real-World Example: WiFi Password Storage

See `WiFi_Manager.cpp` for a complete implementation. Key points:

**Saving Multiple Values:**
```cpp
void savePasswordsToFlash() {
    if (!preferences.begin("wifi", false)) {
        Serial.println("Failed to open Preferences for writing");
        return;
    }
    
    // Save count
    size_t bytesWritten = preferences.putInt("pass_count", passwordCount);
    Serial.printf("Saved pass_count: %d bytes written\n", bytesWritten);
    
    // Save each password
    for (int i = 0; i < passwordCount; i++) {
        String key = "pass_" + String(i);
        bytesWritten = preferences.putString(key.c_str(), passwords[i]);
        Serial.printf("Saved %s: [%s] - %d bytes\n", 
                     key.c_str(), passwords[i].c_str(), bytesWritten);
    }
    
    preferences.end();
}
```

**Loading Multiple Values:**
```cpp
void loadPasswordsFromFlash() {
    if (!preferences.begin("wifi", true)) {
        Serial.println("Failed to open Preferences for reading");
        return;
    }
    
    passwordCount = preferences.getInt("pass_count", 0);
    Serial.printf("Loading %d passwords from flash\n", passwordCount);
    
    for (int i = 0; i < passwordCount && i < MAX_PASSWORDS; i++) {
        String key = "pass_" + String(i);
        passwords[i] = preferences.getString(key.c_str(), "");
        Serial.printf("Loaded %s: [%s]\n", key.c_str(), passwords[i].c_str());
    }
    
    preferences.end();
}
```

### Supported Data Types

| Type | Write Method | Read Method | Example |
|------|-------------|-------------|---------|
| `int8_t` | `putChar(key, value)` | `getChar(key, default)` | `-128 to 127` |
| `uint8_t` | `putUChar(key, value)` | `getUChar(key, default)` | `0 to 255` |
| `int16_t` | `putShort(key, value)` | `getShort(key, default)` | `-32768 to 32767` |
| `uint16_t` | `putUShort(key, value)` | `getUShort(key, default)` | `0 to 65535` |
| `int32_t` | `putInt(key, value)` | `getInt(key, default)` | `±2 billion` |
| `uint32_t` | `putUInt(key, value)` | `getUInt(key, default)` | `0 to 4 billion` |
| `int64_t` | `putLong(key, value)` | `getLong(key, default)` | Very large |
| `uint64_t` | `putULong(key, value)` | `getULong(key, default)` | Very large |
| `float` | `putFloat(key, value)` | `getFloat(key, default)` | `3.14159` |
| `double` | `putDouble(key, value)` | `getDouble(key, default)` | High precision |
| `bool` | `putBool(key, value)` | `getBool(key, default)` | `true/false` |
| `String` | `putString(key, value)` | `getString(key, default)` | Text data |
| `const char*` | `putString(key, value)` | `getString(key, default)` | Text data |
| `byte[]` | `putBytes(key, value, len)` | `getBytes(key, buf, len)` | Binary data |

### Utility Functions

**Check if Key Exists:**
```cpp
if (preferences.isKey("brightness")) {
    int value = preferences.getInt("brightness");
}
```

**Remove a Key:**
```cpp
preferences.remove("old_setting");
```

**Clear All Keys in Namespace:**
```cpp
preferences.clear();  // Deletes all keys in current namespace
```

**Get Free Entries:**
```cpp
size_t freeEntries = preferences.freeEntries();
Serial.printf("Free NVS entries: %d\n", freeEntries);
```

### Namespaces

Organize data into different namespaces (max 15 characters):

```cpp
// WiFi settings
preferences.begin("wifi", false);
preferences.putString("ssid", "MyNetwork");
preferences.end();

// Display settings
preferences.begin("display", false);
preferences.putInt("brightness", 80);
preferences.end();

// Audio settings
preferences.begin("audio", false);
preferences.putFloat("volume", 0.75);
preferences.end();
```

### Best Practices

✅ **Do**: Initialize NVS first
```cpp
esp_err_t err = nvs_flash_init();
// Handle errors before using Preferences
```

✅ **Do**: Check begin() return value
```cpp
if (!preferences.begin("myapp", false)) {
    Serial.println("Preferences failed!");
    return;
}
```

✅ **Do**: Use read-only mode when only reading
```cpp
preferences.begin("settings", true);  // true = read-only, faster
```

✅ **Do**: Always call end()
```cpp
preferences.begin("myapp", false);
preferences.putInt("value", 42);
preferences.end();  // Don't forget!
```

✅ **Do**: Provide sensible defaults
```cpp
int brightness = preferences.getInt("brightness", 50);  // 50 if not found
```

❌ **Don't**: Keep Preferences open too long
```cpp
// Bad - keeps flash locked
preferences.begin("settings", false);
delay(5000);  // Don't do this!
preferences.putInt("value", 42);
preferences.end();
```

❌ **Don't**: Write on every loop iteration
```cpp
// Bad - wears out flash
void loop() {
    preferences.begin("data", false);
    preferences.putInt("counter", counter++);  // Flash has limited write cycles!
    preferences.end();
}
```

❌ **Don't**: Use keys longer than 15 characters
```cpp
// Bad - key too long
preferences.putInt("this_is_a_very_long_key_name", 42);  // Will fail!

// Good - short key
preferences.putInt("longkey", 42);
```

### Flash Storage Limits

- **Namespace Name**: Max 15 characters
- **Key Name**: Max 15 characters  
- **String Value**: Max ~4000 bytes per key
- **Write Cycles**: ~100,000 writes per sector
- **Total NVS Size**: Default 20KB (configurable in partition table)

### Example: Settings Tab with Flash Storage

```cpp
static lv_obj_t * brightness_slider;
static lv_obj_t * volume_slider;
static int current_brightness = 50;
static float current_volume = 0.5;

static void settings_event_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    
    if (code == LV_EVENT_VALUE_CHANGED) {
        if (obj == brightness_slider) {
            current_brightness = lv_slider_get_value(obj);
            
            // Save to flash
            preferences.begin("settings", false);
            preferences.putInt("brightness", current_brightness);
            preferences.end();
            
            Serial.printf("Brightness saved: %d\n", current_brightness);
        }
        else if (obj == volume_slider) {
            current_volume = lv_slider_get_value(obj) / 100.0;
            
            // Save to flash
            preferences.begin("settings", false);
            preferences.putFloat("volume", current_volume);
            preferences.end();
            
            Serial.printf("Volume saved: %.2f\n", current_volume);
        }
    }
}

static void Settings_create(lv_obj_t * parent) {
    // Load saved settings
    preferences.begin("settings", true);
    current_brightness = preferences.getInt("brightness", 50);
    current_volume = preferences.getFloat("volume", 0.5);
    preferences.end();
    
    lv_obj_t * panel = lv_obj_create(parent);
    
    // Brightness slider
    lv_obj_t * bright_label = lv_label_create(panel);
    lv_label_set_text(bright_label, "Brightness");
    
    brightness_slider = lv_slider_create(panel);
    lv_slider_set_range(brightness_slider, 0, 100);
    lv_slider_set_value(brightness_slider, current_brightness, LV_ANIM_OFF);
    lv_obj_add_event_cb(brightness_slider, settings_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Volume slider
    lv_obj_t * vol_label = lv_label_create(panel);
    lv_label_set_text(vol_label, "Volume");
    
    volume_slider = lv_slider_create(panel);
    lv_slider_set_range(volume_slider, 0, 100);
    lv_slider_set_value(volume_slider, (int)(current_volume * 100), LV_ANIM_OFF);
    lv_obj_add_event_cb(volume_slider, settings_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
}
```

---

## Resources

- **LVGL Documentation**: https://docs.lvgl.io/8.3/
- **LVGL Widgets**: https://docs.lvgl.io/8.3/widgets/index.html
- **ESP32 Preferences Library**: https://github.com/espressif/arduino-esp32/tree/master/libraries/Preferences
- **Examples**: See `Onboard_create()` and `Music_create()` in `LVGL_Example.cpp`
- **WiFi Flash Storage**: See `WiFi_Manager.cpp` for complete implementation
- **LVGL Forum**: https://forum.lvgl.io/

---

## Advanced: Charts and Data Visualization

LVGL provides powerful chart widgets for displaying data trends.

### Line Chart Example

```cpp
static lv_obj_t * chart;
static lv_chart_series_t * ser1;

static void Chart_create(lv_obj_t * parent) {
    lv_obj_t * panel = lv_obj_create(parent);
    
    // Create chart
    chart = lv_chart_create(panel);
    lv_obj_set_size(chart, 300, 200);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    
    // Configure chart
    lv_chart_set_point_count(chart, 20);  // 20 data points
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 100);
    lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);  // Scroll left
    
    // Add a series
    ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    
    // Initialize with random data
    for(int i = 0; i < 20; i++) {
        lv_chart_set_next_value(chart, ser1, random(0, 100));
    }
}

// Update chart with new data (call periodically)
void updateChart(int newValue) {
    if (chart != NULL) {
        lv_chart_set_next_value(chart, ser1, newValue);
        lv_chart_refresh(chart);
    }
}
```

### Bar Chart Example

```cpp
static void BarChart_create(lv_obj_t * parent) {
    lv_obj_t * chart = lv_chart_create(parent);
    lv_obj_set_size(chart, 300, 200);
    lv_chart_set_type(chart, LV_CHART_TYPE_BAR);
    lv_chart_set_point_count(chart, 5);
    
    // Add series for different categories
    lv_chart_series_t * ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t * ser2 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    
    // Set values
    lv_chart_set_value_by_id(chart, ser1, 0, 50);
    lv_chart_set_value_by_id(chart, ser1, 1, 70);
    lv_chart_set_value_by_id(chart, ser1, 2, 60);
    lv_chart_set_value_by_id(chart, ser2, 0, 30);
    lv_chart_set_value_by_id(chart, ser2, 1, 90);
    lv_chart_set_value_by_id(chart, ser2, 2, 40);
}
```

---

## Advanced: Arc (Circular Progress/Gauge)

Perfect for circular displays like the 1.85" screen!

### Circular Progress Indicator

```cpp
static lv_obj_t * arc_progress;

static void Arc_create(lv_obj_t * parent) {
    lv_obj_t * panel = lv_obj_create(parent);
    
    // Create arc
    arc_progress = lv_arc_create(panel);
    lv_obj_set_size(arc_progress, 250, 250);
    lv_arc_set_rotation(arc_progress, 135);  // Start from top-left
    lv_arc_set_bg_angles(arc_progress, 0, 270);  // 3/4 circle
    lv_arc_set_value(arc_progress, 75);  // 75%
    
    // Style the arc
    lv_obj_set_style_arc_width(arc_progress, 20, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc_progress, 20, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc_progress, lv_palette_main(LV_PALETTE_BLUE), LV_PART_INDICATOR);
    
    // Remove knob (for pure progress indicator)
    lv_obj_remove_style(arc_progress, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(arc_progress, LV_OBJ_FLAG_CLICKABLE);
    
    // Add label in center
    lv_obj_t * label = lv_label_create(arc_progress);
    lv_label_set_text(label, "75%");
    lv_obj_center(label);
}

// Update progress
void updateProgress(int percent) {
    if (arc_progress != NULL) {
        lv_arc_set_value(arc_progress, percent);
    }
}
```

### Interactive Gauge/Dial

```cpp
static void gauge_event_handler(lv_event_t * e) {
    lv_obj_t * arc = lv_event_get_target(e);
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
        int value = lv_arc_get_value(arc);
        Serial.printf("Gauge value: %d\n", value);
        
        // Save to flash
        preferences.begin("settings", false);
        preferences.putInt("gauge_value", value);
        preferences.end();
    }
}

static void Gauge_create(lv_obj_t * parent) {
    lv_obj_t * gauge = lv_arc_create(parent);
    lv_obj_set_size(gauge, 200, 200);
    lv_arc_set_range(gauge, 0, 100);
    lv_arc_set_value(gauge, 50);
    lv_obj_add_event_cb(gauge, gauge_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Make it draggable
    lv_obj_set_style_arc_color(gauge, lv_palette_main(LV_PALETTE_GREEN), LV_PART_INDICATOR);
}
```

---

## Advanced: Spinbox (Numeric Input)

For precise numeric input with increment/decrement buttons.

```cpp
static lv_obj_t * spinbox;

static void spinbox_increment_event_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
        lv_spinbox_increment(spinbox);
    }
}

static void spinbox_decrement_event_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
        lv_spinbox_decrement(spinbox);
    }
}

static void Spinbox_create(lv_obj_t * parent) {
    lv_obj_t * panel = lv_obj_create(parent);
    
    // Create spinbox
    spinbox = lv_spinbox_create(panel);
    lv_spinbox_set_range(spinbox, 0, 100);
    lv_spinbox_set_digit_format(spinbox, 3, 0);  // 3 digits, 0 decimals
    lv_spinbox_set_step(spinbox, 1);  // Increment by 1
    lv_spinbox_set_value(spinbox, 50);
    lv_obj_set_width(spinbox, 150);
    
    // Create increment button
    lv_obj_t * btn_plus = lv_btn_create(panel);
    lv_obj_set_size(btn_plus, 50, 50);
    lv_obj_t * label_plus = lv_label_create(btn_plus);
    lv_label_set_text(label_plus, "+");
    lv_obj_center(label_plus);
    lv_obj_add_event_cb(btn_plus, spinbox_increment_event_handler, LV_EVENT_ALL, NULL);
    
    // Create decrement button
    lv_obj_t * btn_minus = lv_btn_create(panel);
    lv_obj_set_size(btn_minus, 50, 50);
    lv_obj_t * label_minus = lv_label_create(btn_minus);
    lv_label_set_text(label_minus, "-");
    lv_obj_center(label_minus);
    lv_obj_add_event_cb(btn_minus, spinbox_decrement_event_handler, LV_EVENT_ALL, NULL);
    
    // Layout
    static lv_coord_t col_dsc[] = {50, 150, 50, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {50, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(panel, col_dsc, row_dsc);
    
    lv_obj_set_grid_cell(btn_minus, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(spinbox, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(btn_plus, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 0, 1);
}

// Get spinbox value
int getSpinboxValue() {
    return lv_spinbox_get_value(spinbox);
}
```

---

## Advanced: Roller (Scrollable Picker)

Great for selecting from a list of options.

```cpp
static lv_obj_t * roller;

static void roller_event_handler(lv_event_t * e) {
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
        char buf[32];
        lv_roller_get_selected_str(roller, buf, sizeof(buf));
        Serial.printf("Selected: %s\n", buf);
        
        uint16_t selected = lv_roller_get_selected(roller);
        Serial.printf("Index: %d\n", selected);
    }
}

static void Roller_create(lv_obj_t * parent) {
    lv_obj_t * panel = lv_obj_create(parent);
    
    // Create roller
    roller = lv_roller_create(panel);
    lv_roller_set_options(roller, 
        "WiFi\n"
        "Bluetooth\n"
        "Ethernet\n"
        "Cellular\n"
        "LoRa\n"
        "Zigbee\n"
        "Thread",
        LV_ROLLER_MODE_NORMAL);  // or LV_ROLLER_MODE_INFINITE for infinite scroll
    
    lv_roller_set_visible_row_count(roller, 4);  // Show 4 rows at once
    lv_obj_set_width(roller, 200);
    lv_obj_add_event_cb(roller, roller_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Set initial selection
    lv_roller_set_selected(roller, 0, LV_ANIM_OFF);
}
```

---

## Advanced: LED Indicator

Simple LED-style indicator for status display.

```cpp
static lv_obj_t * led_red;
static lv_obj_t * led_green;
static lv_obj_t * led_blue;

static void LED_create(lv_obj_t * parent) {
    lv_obj_t * panel = lv_obj_create(parent);
    
    // Create LEDs
    led_red = lv_led_create(panel);
    lv_obj_set_size(led_red, 50, 50);
    lv_led_set_color(led_red, lv_palette_main(LV_PALETTE_RED));
    lv_led_on(led_red);
    
    led_green = lv_led_create(panel);
    lv_obj_set_size(led_green, 50, 50);
    lv_led_set_color(led_green, lv_palette_main(LV_PALETTE_GREEN));
    lv_led_off(led_green);
    
    led_blue = lv_led_create(panel);
    lv_obj_set_size(led_blue, 50, 50);
    lv_led_set_color(led_blue, lv_palette_main(LV_PALETTE_BLUE));
    lv_led_off(led_blue);
    
    // Add labels
    lv_obj_t * label_r = lv_label_create(panel);
    lv_label_set_text(label_r, "Error");
    
    lv_obj_t * label_g = lv_label_create(panel);
    lv_label_set_text(label_g, "Ready");
    
    lv_obj_t * label_b = lv_label_create(panel);
    lv_label_set_text(label_b, "Busy");
}

// Control LEDs
void setStatus(const char* status) {
    if (strcmp(status, "error") == 0) {
        lv_led_on(led_red);
        lv_led_off(led_green);
        lv_led_off(led_blue);
    } else if (strcmp(status, "ready") == 0) {
        lv_led_off(led_red);
        lv_led_on(led_green);
        lv_led_off(led_blue);
    } else if (strcmp(status, "busy") == 0) {
        lv_led_off(led_red);
        lv_led_off(led_green);
        lv_led_on(led_blue);
    }
}

// Toggle LED
void toggleLED() {
    lv_led_toggle(led_green);
}

// Set LED brightness (0-255)
void setLEDBrightness(uint8_t brightness) {
    lv_led_set_brightness(led_green, brightness);
}
```

---

## Advanced: Message Box (Modal Dialog)

Create popup dialogs for confirmations and alerts.

```cpp
static void msgbox_event_handler(lv_event_t * e) {
    lv_obj_t * obj = lv_event_get_current_target(e);
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
        const char * txt = lv_msgbox_get_active_btn_text(obj);
        if (txt) {
            Serial.printf("Button clicked: %s\n", txt);
            
            if (strcmp(txt, "Delete") == 0) {
                // Perform delete action
                Serial.println("Deleting...");
            }
            
            // Close the message box
            lv_msgbox_close(obj);
        }
    }
}

void showMessageBox(const char* title, const char* message) {
    static const char * btns[] = {"OK", "Cancel", ""};
    
    lv_obj_t * mbox = lv_msgbox_create(NULL, title, message, btns, true);
    lv_obj_add_event_cb(mbox, msgbox_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_center(mbox);
}

// Delete confirmation dialog
void showDeleteConfirmation() {
    static const char * btns[] = {"Delete", "Cancel", ""};
    
    lv_obj_t * mbox = lv_msgbox_create(NULL, 
        "Confirm Delete", 
        "Are you sure you want to delete this item?", 
        btns, true);
    lv_obj_add_event_cb(mbox, msgbox_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_center(mbox);
}
```

---

## Advanced: Keyboard (On-Screen)

Display an on-screen keyboard for text input.

```cpp
static lv_obj_t * keyboard;
static lv_obj_t * textarea_input;

static void keyboard_event_handler(lv_event_t * e) {
    lv_obj_t * kb = lv_event_get_target(e);
    lv_keyboard_def_event_cb(e);  // Call default handler first
    
    if (lv_event_get_code(e) == LV_EVENT_READY || lv_event_get_code(e) == LV_EVENT_CANCEL) {
        // Get the text
        const char * text = lv_textarea_get_text(textarea_input);
        Serial.printf("Input: %s\n", text);
        
        // Save to flash
        preferences.begin("input", false);
        preferences.putString("last_input", text);
        preferences.end();
        
        // Hide keyboard
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}

static void textarea_event_handler(lv_event_t * e) {
    if (lv_event_get_code(e) == LV_EVENT_FOCUSED) {
        // Show keyboard when textarea is focused
        if (keyboard != NULL) {
            lv_obj_clear_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

static void Keyboard_create(lv_obj_t * parent) {
    lv_obj_t * panel = lv_obj_create(parent);
    
    // Create textarea
    textarea_input = lv_textarea_create(panel);
    lv_obj_set_size(textarea_input, 300, 80);
    lv_textarea_set_placeholder_text(textarea_input, "Enter text...");
    lv_textarea_set_one_line(textarea_input, false);  // Multi-line
    lv_textarea_set_max_length(textarea_input, 100);
    lv_obj_add_event_cb(textarea_input, textarea_event_handler, LV_EVENT_FOCUSED, NULL);
    
    // Create keyboard
    keyboard = lv_keyboard_create(panel);
    lv_obj_set_size(keyboard, 340, 200);
    lv_keyboard_set_textarea(keyboard, textarea_input);  // Link to textarea
    lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_TEXT_LOWER);  // Lowercase mode
    lv_obj_add_event_cb(keyboard, keyboard_event_handler, LV_EVENT_ALL, NULL);
    
    // Initially hide keyboard
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
}

// Show keyboard with specific mode
void showKeyboard(lv_keyboard_mode_t mode) {
    if (keyboard != NULL) {
        lv_keyboard_set_mode(keyboard, mode);  // LV_KEYBOARD_MODE_NUMBER, etc.
        lv_obj_clear_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    }
}
```

---

## Quick Reference Checklist

When adding a new tab:
- [ ] Add function declaration at top of `LVGL_Example.cpp`
- [ ] Call `lv_tabview_add_tab()` in `Lvgl_Example1()`
- [ ] Call your `YourTab_create()` function after creating tab
- [ ] Implement `YourTab_create(lv_obj_t * parent)` function
- [ ] Create panel with `lv_obj_create(parent)`
- [ ] Add title label with `style_title`
- [ ] Add your widgets (labels, buttons, sliders, etc.)
- [ ] Set up grid layout
- [ ] Add event callbacks if needed
- [ ] Declare global pointers for widgets that need updates
- [ ] Create update functions in header file
- [ ] Update `auto_switch()` if adding to auto-rotation
- [ ] Test on hardware!

### Flash Storage Checklist

When using Preferences:
- [ ] Include `<Preferences.h>` and `<nvs_flash.h>` in header
- [ ] Call `nvs_flash_init()` in setup/constructor
- [ ] Handle NVS initialization errors
- [ ] Check `preferences.begin()` return value
- [ ] Use descriptive namespace (max 15 chars)
- [ ] Use short key names (max 15 chars)
- [ ] Provide sensible default values
- [ ] Always call `preferences.end()` after operations
- [ ] Don't write in tight loops (flash wear)
- [ ] Use read-only mode when only reading
- [ ] Test persistence across reboots

---

*Generated for ESP32-S3 Touch LCD 1.85" (360x360) - LVGL v8.3.0*

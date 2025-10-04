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
```cpp
lv_obj_t * label = lv_label_create(parent);
lv_label_set_text(label, "Your Text");
lv_obj_add_style(label, &style_text_muted, 0);  // Apply muted text style
```

#### 2. Text Areas (Editable/Display Fields)
```cpp
lv_obj_t * textarea = lv_textarea_create(parent);
lv_textarea_set_one_line(textarea, true);
lv_textarea_set_placeholder_text(textarea, "Placeholder");
lv_textarea_set_text(textarea, "Content");  // Update content later
```

#### 3. Buttons
```cpp
lv_obj_t * btn = lv_btn_create(parent);
lv_obj_t * btn_label = lv_label_create(btn);
lv_label_set_text(btn_label, "Click Me");
lv_obj_add_event_cb(btn, button_event_cb, LV_EVENT_CLICKED, NULL);
```

#### 4. Sliders
```cpp
lv_obj_t * slider = lv_slider_create(parent);
lv_obj_set_size(slider, 200, 35);
lv_slider_set_range(slider, 0, 100);
lv_slider_set_value(slider, 50, LV_ANIM_ON);
lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
```

#### 5. Switches
```cpp
lv_obj_t * sw = lv_switch_create(parent);
lv_obj_add_event_cb(sw, switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
```

#### 6. Checkboxes
```cpp
lv_obj_t * cb = lv_checkbox_create(parent);
lv_checkbox_set_text(cb, "Enable Feature");
```

#### 7. Arcs (Circular Progress/Input)
```cpp
lv_obj_t * arc = lv_arc_create(parent);
lv_arc_set_range(arc, 0, 100);
lv_arc_set_value(arc, 50);
lv_arc_set_rotation(arc, 135);
lv_arc_set_bg_angles(arc, 0, 270);
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
- `LV_EVENT_ALL` - Listen to all events

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

## Resources

- **LVGL Documentation**: https://docs.lvgl.io/8.3/
- **LVGL Widgets**: https://docs.lvgl.io/8.3/widgets/index.html
- **Examples**: See `Onboard_create()` and `Music_create()` in `LVGL_Example.cpp`
- **LVGL Forum**: https://forum.lvgl.io/

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

---

*Generated for ESP32-S3 Touch LCD 1.85" (360x360) - LVGL v8.3.0*

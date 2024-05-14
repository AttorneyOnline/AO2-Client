#pragma once

#define FROM_UI(type, name) ui_##name = findChild<type *>(#name)

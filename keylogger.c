// originally from https://github.com/caseyscarborough/keylogger

#include <ApplicationServices/ApplicationServices.h>

CGEventRef CGEventCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon) {
  CGEventFlags flags = CGEventGetFlags(event);
  CGKeyCode keyCode = (CGKeyCode) CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);

  if(CGEventGetIntegerValueField(event, kCGKeyboardEventAutorepeat) != 0
     || flags & kCGEventFlagMaskCommand
     || flags & kCGEventFlagMaskAlternate
     || flags & kCGEventFlagMaskControl)
    {
      return event;
    }

  struct timespec time_struct;
  clock_gettime(CLOCK_MONOTONIC_RAW, &time_struct);

  uint64_t microseconds = time_struct.tv_sec * 1000000 + time_struct.tv_nsec / 1000;

  printf(", %d %d %llu", keyCode, type == kCGEventKeyDown, microseconds); // (unsigned long)time(NULL));
  fflush(stdout);
  return event;
}

int main(int argc, const char *argv[]) {

  CFMachPortRef eventTap = CGEventTapCreate(kCGSessionEventTap,
                                            kCGHeadInsertEventTap,
                                            0,
                                            CGEventMaskBit(kCGEventKeyUp) | CGEventMaskBit(kCGEventKeyDown),
                                            CGEventCallback, NULL);

  if (!eventTap) {
    fprintf(stderr, "ERROR: Unable to create event tap.\n");
    exit(1);
  }

  CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
  CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
  CGEventTapEnable(eventTap, true);

  CFRunLoopRun();

  return 0;
}

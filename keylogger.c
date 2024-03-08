#include <ApplicationServices/ApplicationServices.h>

CGEventRef CGEventCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon) {
  CGEventFlags flags = CGEventGetFlags(event);

  if(CGEventGetIntegerValueField(event, kCGKeyboardEventAutorepeat) != 0
     || flags & kCGEventFlagMaskCommand
     || flags & kCGEventFlagMaskAlternate
     || flags & kCGEventFlagMaskControl)
    {
      return event;
    }

  struct timespec time_now;
  clock_gettime(CLOCK_MONOTONIC_RAW, &time_now);

  printf(", %llu %lu",
         CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode),
         time_now.tv_sec * 1000000 + time_now.tv_nsec / 1000);
  fflush(stdout);
  return event;
}

int main(int argc, const char *argv[]) {

  CFMachPortRef eventTap = CGEventTapCreate(kCGSessionEventTap,
                                            kCGHeadInsertEventTap,
                                            0,
                                            CGEventMaskBit(kCGEventKeyDown),
                                            CGEventCallback,
                                            NULL);

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

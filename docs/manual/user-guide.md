# User Guide {#user_guide}

\brief Comprehensive guide to using the Kit library components and features.

[TOC]

@note **⚠️ UNDER CONSTRUCTION** - This documentation is currently being updated.

## Overview

The KIT Library is a comprehensive suite of C++ middleware specifically designed for developing microcontroller
applications that “work” with any microcontroller and operating systems.  Or said another way, KIT is platform
agnostic.  Because the library is purpose built to be platform independent – the following features are “free”
out-of-the-box:

* Automated unit tests that execute as terminal applications on Windows and Linux, i.e. CI/CD ready.  In practice
  over 80% of your application can be unit test this way.
* Functional simulator for your application on Day 0.  Since +80% of your application can be unit tested off
  target – building a function simulator is minimal effort (i.e. it is more of planning effort that a coding effort).
  * What is Day 0?  You can stand up the functional simulator before you have built, purchased, or even defined your
    hardware platform.  This means you can write and test production code as soon as the project starts. **No more
    gating firmware development by HW availability**.
* Moving to a new Microcontroller, Microprocessor, or new PCBA is a low-risk activity because none of the application’s
  business logic is dependent on hardware specific entities.  
  * This means that only the lowest drivers are impacted when transitioning to a new hardware platform.  For example,
    if you started the project with a function simulator your application already runs on least two platforms – so
    the mechanics of supporting yet another platform has already been defined/laid out.

## Core Concepts

### Memory Management

Kit uses static allocation strategies to avoid dynamic memory allocation:

* Limitless containers that use intrusive linkage
* Usage of new/malloc restricted to start up logic

### Platform Independent

## NQBP Build Engine

## Outcast Repository Manager

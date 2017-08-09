Getting Started with VR View in HTML 
============================================

This is the companion project to the VR View in HTML codelab.

Please see https://codelabs.developers.google.com/ for the codelab text.

Support
-------
 If you've found an error in this sample, please file an issue:
https://github.com/googlecodelabs/vrview-html/issues

Patches are encouraged, and may be submitted by forking this project and
submitting a pull request through GitHub.

License
-------
Copyright 2016 Google, Inc.
Licensed to the Apache Software Foundation (ASF) under one or more contributor
license agreements.  See the NOTICE file distributed with this work for
additional information regarding copyright ownership.  The ASF licenses this
file to you under the Apache License, Version 2.0 (the "License"); you may not
use this file except in compliance with the License.  You may obtain a copy of
the License at
  http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
License for the specific language governing permissions and limitations under
the License.

# VR View

[![Build Status](https://travis-ci.org/googlevr/vrview.svg?branch=master)](https://travis-ci.org/googlevr/vrview)
[![dependencies Status](https://david-dm.org/googlevr/vrview/status.svg)](https://david-dm.org/googlevr/vrview)
[![devDependencies Status](https://david-dm.org/googlevr/vrview/dev-status.svg)](https://david-dm.org/googlevr/vrview?type=dev)

VR View allows you to embed 360 degree VR media into websites on desktop and
mobile. For more information, please read the documentation available at
<http://developers.google.com/cardboard/vrview>.

# Configuration

A complete list of VR View parameters can be found in the table below.

Name | Type | Parameter description
---- | ---- | ---------------------
`video` | String | URL to a 360° video file or an adaptive streaming manifest file (.mpd or .m3u8). Exactly one of video or image is required.
`image` | String | URL to a 360° image file. Exactly one of video or image is required.
`width` | String | String value for the iframe's width attribute.
`height` | String | String value for the iframe's height attribute.
`preview` | String | (Optional) URL to a preview image for a 360° image file.
`is_stereo` | Boolean | (Optional) Indicates whether the content at the image or video URL is stereo or not.
`is_debug` | Boolean | (Optional) When true, turns on debug features like rendering hotspots ad showing the FPS meter.
`is_vr_off` | Boolean | (Optional) When true, disables the VR mode button.
`is_autopan_off` | Boolean | (Optional) When true, disables the autopan introduction on desktop.
`default_yaw` | Number | (Optional) Numeric angle in degrees of the initial heading for the 360° content. By default, the camera points at the center of the underlying image.
`is_yaw_only` | Boolean | (Optional) When true, prevents roll and pitch. This is intended for stereo panoramas.
`loop` | Boolean | (Optional) When false, stops the loop in the video.
`hide_fullscreen_button` | Boolean | (Optional) When true, the fullscreen button contained inside the VR View iframe will be hidden. This parameter is useful if the user wants to use VR View's fullscreen workflow (via `vrView.setFullscreen()` callback) with an element outside the iframe. 
`volume` | Number | (Optional) The initial volume of the media; it ranges between 0 and 1; zero equals muted.
`muted` | Boolean | (Optional) When true, mutes the sound of the video.

# Building

This project uses `browserify` to manage dependencies and build. `watchify` is
especially convenient to preserve the write-and-reload model of development.
This package lives in the npm index.

Relevant commands:

$ npm install # install the npm

$ npm run build # builds the iframe embed and JS API (full and minified versions).

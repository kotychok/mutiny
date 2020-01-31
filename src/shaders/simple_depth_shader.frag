#version 330 core

void main() {
  // Since we have no color buffer the resulting fragments do not require any
  // processing, so we can simply use an empty fragment shader.
  //
  // The empty fragment shader does no processing whatsoever and at the end of
  // its run the depth buffer is updated.
}

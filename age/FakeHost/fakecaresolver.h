#pragma once

// Resolves the real function targets. Returns false when the resolved targets
// are incomplete, in which case the attach below must not be called.
bool FakeCAResolverInit();

void FakeCAResolverAttach();
void FakeCAResolverDetach();

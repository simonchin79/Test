# memory.md — Project Memory

This file stores durable project memory that persists across agent sessions.
It is always loaded as context so the agent can recall past work,
decisions, open issues, and next steps.

## Status
- `classify.py` created — two-stage classifier using mean brightness (Stage 1) and histogram entropy (Stage 2).
- Can classify single images or entire directories of images.

## Decisions
- Stage 1 threshold: brightness > 75 → P50.
- Stage 2 threshold: entropy > 3.62 → P80, else P150 (32-bin histogram).
- Fallback secondary features (Sobel mean gradient, mid-range pixel %) documented in implementation.md but not yet implemented in classify.py.

## Open Issues / Next Steps
- Evaluate accuracy on the actual Dataset/ folders.
- Consider adding the secondary fallback features (Sobel mean gradient, mid-range pixel %) to improve P80/P150 accuracy beyond ~95.6%.
- Add unit tests.

# 16KB Sine – L1 Cache Friendly, Near-Hardware Accuracy

**Max error:** 1.25e-06  
**RMS error:** 6.13e-07  
**Memory:** 16KB total (fits in L1 cache)  
**License:** MIT

## Why This Exists

This is a residual-corrected sine lookup table. It uses two 2048-entry tables (`L0` + `R0`) to achieve near‑perfect accuracy using only linear interpolation. The residual table corrects the interpolation error of the base table to floating‑point precision.

## How It Works

1. `L0` stores `sin(x)` at 2048 evenly spaced points.
2. `R0` stores the error between the true `sin(x)` and the interpolated value from `L0`.
3. Evaluation = `interp(L0, x) + interp(R0, x)`.

The result matches Apple's hardware `sinf()` in accuracy, while fitting entirely in L1 cache (16KB).

## Performance

- **Speed:** Fast (table lookup + linear interpolation, no hardware `sinf` call)
- **Memory:** 16KB (8KB for L0 + 8KB for R0)
- **Accuracy:** Max error 1.25e-06, RMS 6.13e-07

## Files

- `residual_lut_v2.cpp` – Complete implementation with benchmark

## License

This project is licensed under the MIT License – see the [LICENSE](LICENSE) file for details.

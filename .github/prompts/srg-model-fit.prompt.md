---
mode: agent
description: Guides Software Reliability Growth (SRG) model selection and fitting following IEEE 1633-2016 Clause 5.4, 6.3
---

# Software Reliability Growth (SRG) Model Fitting Prompt

You are an **SRG Modeling Expert** following **IEEE 1633-2016** Clause 5.4, 6.3 for reliability growth analysis.

## 📤 EXPECTED OUTPUT (ALWAYS DELIVER)

When user requests SRG model fitting, you **MUST** produce a complete SRG analysis report with model parameters, predictions, and release recommendations.

### Complete SRG Analysis Report Structure

```markdown
# Software Reliability Growth (SRG) Analysis Report

**Project**: [Project Name]
**Version**: [X.Y.Z]
**Date**: [YYYY-MM-DD]
**Document ID**: SRG-[Component]-[Version]
**Status**: [Draft/Review/Approved]
**IEEE 1633-2016 Compliant** (Clause 5.4, 6.3)

---

## Document Control

| Version | Date | Author | Changes | Approved By |
|---------|------|--------|---------|-------------|
| [X.Y] | [Date] | [Name] | [Summary] | [Name] |

## Table of Contents
1. Introduction
2. Failure Data Summary
3. Trend Analysis
4. Model Selection and Fitting
5. Goodness-of-Fit Assessment
6. Reliability Predictions
7. Release Recommendations
8. Model Validation

---

## 1. Introduction

### 1.1 Purpose
This SRG analysis uses failure data from testing to:
- **Fit multiple SRG models** (Musa-Okumoto, Goel-Okumoto, Jelinski-Moranda, Crow/AMSAA)
- **Assess goodness-of-fit** (which model best represents the data)
- **Estimate current reliability** (MTBF, failure rate)
- **Predict future reliability** (residual defects, time to target MTBF)
- **Support release decision** (is software ready to release?)

### 1.2 SRG Concept (IEEE 1633 Clause 6.3)

**Software Reliability Growth (SRG)** is the phenomenon where:
- As defects are detected and fixed, reliability **increases** over time
- Failure rate **decreases** over time
- MTBF **increases** over time

**SRG Models** mathematically describe this growth process, enabling predictions.

### 1.3 Relationship to Other Documents
- **Source Documents**:
  - Operational Profile: [OP-Component-X.Y.md]
  - Reliability Test Plan: [RTP-Component-X.Y.md]
  - Test Results/Failure Data: [Test execution log]
  - SRPP: [SRPP-Project-X.Y.md] (Section 7 - SRG Testing)
- **Output Documents**:
  - SRG Analysis Report: This document
  - Release Decision Report: [Use `reliability-release-decision` prompt]

---

## 2. Failure Data Summary

### 2.1 Test Execution Summary
- **Test Start Date**: [Date]
- **Test End Date**: [Date]
- **Total Test Time**: [T hours] or [T CPU-hours]
- **Total Test Cases Executed**: [N]
- **Total Failures Observed**: [M]
- **Failures Fixed**: [M_fixed]
- **Failures Outstanding**: [M - M_fixed]

### 2.2 Failure Data (Raw)

| Failure # | Failure Time (hours) | Test Case | Operation | Severity (FDSC) | Root Cause | Fixed? | Fix Time (hours) |
|-----------|---------------------|-----------|-----------|----------------|------------|--------|------------------|
| 1 | 10.5 | TC-015 | OP-001 | 10 (Critical) | Null pointer exception | ✅ | 15.0 |
| 2 | 25.3 | TC-042 | OP-002 | 7 (High) | Incorrect query | ✅ | 28.0 |
| 3 | 31.8 | TC-089 | OP-001 | 10 (Critical) | Buffer overflow | ✅ | 40.0 |
| 4 | 45.2 | TC-105 | OP-003 | 4 (Medium) | Timeout | ❌ | - |
| ... | ... | ... | ... | ... | ... | ... | ... |

### 2.3 Time Between Failures (TBF)

| Interval | Failure # | Failure Time | Time Between Failures (TBF) | Cumulative Failures | Cumulative Time |
|----------|-----------|--------------|----------------------------|---------------------|-----------------|
| 1 | 1 | 10.5 | 10.5 | 1 | 10.5 |
| 2 | 2 | 25.3 | 14.8 | 2 | 25.3 |
| 3 | 3 | 31.8 | 6.5 | 3 | 31.8 |
| 4 | 4 | 45.2 | 13.4 | 4 | 45.2 |
| ... | ... | ... | ... | ... | ... |

**Average TBF**: [Mean of TBF column] hours

### 2.4 Failure Intensity (Failures per Hour)

| Time Interval | Failures in Interval | Failure Intensity (λ) |
|---------------|---------------------|-----------------------|
| 0-10 hours | 1 | 0.10 failures/hour |
| 10-20 hours | 1 | 0.10 failures/hour |
| 20-30 hours | 1 | 0.10 failures/hour |
| 30-40 hours | 0 | 0.00 failures/hour |
| 40-50 hours | 1 | 0.10 failures/hour |

**Current Failure Intensity**: [Last interval value] failures/hour

### 2.5 Failure Data Plot

```
Cumulative Failures vs. Test Time

Failures |
   M     |                                 ×
         |                             ×
         |                         ×
         |                     ×
         |                 ×
         |             ×
         |         ×
         |     ×
         | ×
    0    |_________________________________
         0                                 T
                    Test Time (hours)
```

**Visual Inspection**: Cumulative failures show [linear/logarithmic/exponential] growth pattern.

---

## 3. Trend Analysis

### 3.1 Trend Test Purpose (IEEE 1633 Clause 6.3.2)

**Before fitting models**, test if reliability is **actually growing**:
- **Null Hypothesis (H0)**: Failure times are random (no reliability growth)
- **Alternative Hypothesis (H1)**: Failure times show trend (reliability growth or decay)

**If no trend detected**: Reliability not improving, investigate and fix more defects before release.

### 3.2 Laplace Trend Test (IEEE 1633 Clause 6.3.2.1)

**Formula**:
```
u = [Σ(t_i) / M - T/2] / [T * sqrt(1/(12*M))]

Where:
  M = number of failures
  T = total test time
  t_i = time of i-th failure
```

**Interpretation**:
- **u > 2**: Reliability is **decreasing** (failures increasing) - Bad! ❌
- **-2 ≤ u ≤ 2**: **No trend** detected (failures random) - Investigate 🤔
- **u < -2**: Reliability is **increasing** (failures decreasing) - Good! ✅

**Calculation**:
```
M = [Number of failures]
T = [Total test time] hours
Σ(t_i) = [Sum of all failure times]

u = [Σ(t_i) / M - T/2] / [T * sqrt(1/(12*M))]
u = [Calculated value]
```

**Result**: u = [Value]
**Interpretation**: [Reliability is increasing/decreasing/no trend]
**Confidence**: [95% confidence interval]

### 3.3 Arithmetic Mean (AM) Trend Test (IEEE 1633 Clause 6.3.2.2)

**Formula**:
```
AM(i) = Mean of TBF for failures i to M

If AM(i) increases with i → Reliability growing ✅
If AM(i) decreases with i → Reliability declining ❌
```

**Calculation**:

| Group | Failures | Mean TBF | Trend |
|-------|----------|----------|-------|
| 1-5 | 1, 2, 3, 4, 5 | [X] hours | Baseline |
| 6-10 | 6, 7, 8, 9, 10 | [Y] hours | [Increasing/Decreasing] |
| 11-15 | 11, 12, 13, 14, 15 | [Z] hours | [Increasing/Decreasing] |

**Plot**:
```
Mean TBF |
         |         ×
         |     ×
         | ×
         |_____________
           1-5  6-10  11-15
              Failure Groups
```

**Result**: Mean TBF [increasing/decreasing/flat]
**Interpretation**: Reliability [growing/declining/stable]

### 3.4 Trend Test Conclusion

**Laplace Test**: u = [Value] → [Growing/No trend/Declining]
**AM Test**: Mean TBF [Increasing/Flat/Decreasing] → [Growing/Stable/Declining]

**Overall Conclusion**: ✅ Reliability is growing, proceed with model fitting
                        OR
                        ❌ No reliability growth detected, do NOT release, investigate

---

## 4. Model Selection and Fitting

### 4.1 SRG Model Categories (IEEE 1633 Clause 6.3.3)

| Model Type | Assumption | Best For | Examples |
|------------|------------|----------|----------|
| **Finite Failures** | Fixed number of defects (N₀) | Projects with stable requirements | Jelinski-Moranda, Goel-Okumoto |
| **Infinite Failures** | Defects introduced during fixes | Large, evolving systems | Musa-Okumoto |
| **Non-parametric** | No assumption about defect distribution | Early testing, unstable systems | Crow/AMSAA |

### 4.2 Model Selection Criteria

Select 3-4 models to fit based on:
1. **Project characteristics**: Size, maturity, requirement stability
2. **Data characteristics**: Number of failures (M ≥ 20 recommended), test time
3. **Industry practice**: Common models in your domain

**Selected Models for This Analysis**:
- ✅ Musa-Okumoto (Infinite failures, execution time)
- ✅ Goel-Okumoto (Finite failures, calendar time)
- ✅ Jelinski-Moranda (Finite failures, simple)
- ✅ Crow/AMSAA (Non-parametric, power law)

### 4.3 Model Fitting Methods

**Fitting Techniques**:
- **Maximum Likelihood Estimation (MLE)**: Most common, optimal estimates
- **Least Squares**: Simpler, but less accurate
- **Bayesian**: Incorporates prior knowledge

**Tool Used**: [CASRE, SoRel, SMERFS, Python statsmodels, R survival, etc.]

---

### 4.4 Model 1: Musa-Okumoto (Logarithmic Poisson Execution Time Model)

**Type**: Infinite failures model
**Assumptions**:
- Defects introduced during fixes (infinite failures)
- Failure intensity decreases logarithmically
- Execution time basis

**Model Equations**:
```
λ(t) = λ₀ / (1 + θ*t)        [Failure intensity]
μ(t) = (1/θ) * ln(1 + λ₀*θ*t)  [Expected cumulative failures]

Where:
  λ₀ = initial failure intensity (failures/hour)
  θ = failure intensity decay parameter
  t = test time (hours)
```

**Parameter Estimation** (MLE):
```
λ₀ = [Estimated value] failures/hour
θ = [Estimated value]
```

**Current Metrics** (at T = [Current test time] hours):
```
Current Failure Intensity: λ(T) = λ₀ / (1 + θ*T) = [Value] failures/hour
Current MTBF: MTBF(T) = 1 / λ(T) = [Value] hours
Expected Cumulative Failures: μ(T) = [Value] failures
```

**Predictions** (at T + Δt hours):
```
Future Failure Intensity: λ(T + Δt) = [Value] failures/hour
Future MTBF: MTBF(T + Δt) = [Value] hours
Additional Expected Failures in next Δt hours: μ(T + Δt) - μ(T) = [Value]
```

---

### 4.5 Model 2: Goel-Okumoto (Non-Homogeneous Poisson Process - NHPP)

**Type**: Finite failures model
**Assumptions**:
- Fixed initial number of defects (N₀)
- Defects detected exponentially
- Defects fixed immediately (or with lag)

**Model Equations**:
```
μ(t) = N₀ * (1 - e^(-b*t))   [Expected cumulative failures]
λ(t) = N₀ * b * e^(-b*t)       [Failure intensity]

Where:
  N₀ = initial number of defects
  b = defect detection rate (1/hour)
  t = test time (hours)
```

**Parameter Estimation** (MLE):
```
N₀ = [Estimated value] defects
b = [Estimated value] per hour
```

**Current Metrics** (at T = [Current test time] hours):
```
Expected Cumulative Failures: μ(T) = N₀ * (1 - e^(-b*T)) = [Value]
Current Failure Intensity: λ(T) = N₀ * b * e^(-b*T) = [Value] failures/hour
Current MTBF: MTBF(T) = 1 / λ(T) = [Value] hours
Residual Defects: N₀ - μ(T) = [Value] defects remaining
```

**Predictions**:
```
Time to reach target MTBF = [Target] hours:
  Solve: 1 / λ(t) = Target MTBF
  t_target = [Value] hours (additional [t_target - T] hours needed)

Expected failures in next [Δt] hours: μ(T + Δt) - μ(T) = [Value]
```

---

### 4.6 Model 3: Jelinski-Moranda (De-Eutrophication Model)

**Type**: Finite failures model
**Assumptions**:
- Fixed initial number of defects (N₀)
- All defects have equal hazard rate (φ)
- Defects fixed immediately after detection

**Model Equations**:
```
λ_i = φ * (N₀ - (i-1))   [Failure intensity after (i-1) failures]

Where:
  φ = hazard rate per defect (failures/hour/defect)
  N₀ = initial number of defects
  i = failure number
```

**Parameter Estimation** (MLE):
```
N₀ = [Estimated value] defects
φ = [Estimated value] failures/hour/defect
```

**Current Metrics** (after M = [Number of failures detected] failures):
```
Current Failure Intensity: λ_M = φ * (N₀ - M) = [Value] failures/hour
Current MTBF: MTBF = 1 / λ_M = [Value] hours
Residual Defects: N₀ - M = [Value] defects
```

**Predictions**:
```
Time to next failure: E[TBF_(M+1)] = 1 / λ_(M+1) = [Value] hours
Total time to detect all N₀ defects: [Calculated from model] hours
```

---

### 4.7 Model 4: Crow/AMSAA (Power Law Model)

**Type**: Non-parametric growth model
**Assumptions**:
- Failure intensity follows power law
- No assumption about defect distribution
- Commonly used in hardware reliability (also applies to software)

**Model Equations**:
```
λ(t) = λ * β * t^(β-1)     [Failure intensity]
μ(t) = λ * t^β             [Expected cumulative failures]

Where:
  λ = scale parameter
  β = shape parameter (β < 1: reliability growing, β > 1: declining)
  t = test time (hours)
```

**Parameter Estimation** (MLE):
```
λ = [Estimated value]
β = [Estimated value]
```

**Interpretation**:
- β < 1: Reliability **growing** (failure intensity decreasing) ✅
- β = 1: **No growth** (constant failure rate)
- β > 1: Reliability **declining** (failure intensity increasing) ❌

**Current Metrics** (at T = [Current test time] hours):
```
Current Failure Intensity: λ(T) = λ * β * T^(β-1) = [Value] failures/hour
Current MTBF: MTBF(T) = 1 / λ(T) = [Value] hours
Expected Cumulative Failures: μ(T) = λ * T^β = [Value]
```

**Predictions**:
```
Future Failure Intensity (at T + Δt): λ(T + Δt) = [Value] failures/hour
Reliability Growth Rate: β = [Value] ([<1 growing, >1 declining])
```

---

### 4.8 Model Fitting Summary

| Model | Type | Parameters | Current MTBF | Current λ | Residual Defects |
|-------|------|------------|--------------|-----------|------------------|
| **Musa-Okumoto** | Infinite | λ₀=[X], θ=[Y] | [Z] hours | [A] fail/hr | N/A (infinite) |
| **Goel-Okumoto** | Finite | N₀=[N], b=[B] | [Z] hours | [A] fail/hr | [N₀ - μ(T)] |
| **Jelinski-Moranda** | Finite | N₀=[N], φ=[Φ] | [Z] hours | [A] fail/hr | [N₀ - M] |
| **Crow/AMSAA** | Non-param | λ=[L], β=[B] | [Z] hours | [A] fail/hr | N/A |

---

## 5. Goodness-of-Fit Assessment

### 5.1 Goodness-of-Fit Metrics

**Purpose**: Determine which model **best fits** the observed failure data.

| Metric | Description | Ideal Value |
|--------|-------------|-------------|
| **SSE** | Sum of Squared Errors | Minimum |
| **MSE** | Mean Squared Error | Minimum |
| **R²** | Coefficient of Determination | Close to 1.0 |
| **AIC** | Akaike Information Criterion | Minimum |
| **BIC** | Bayesian Information Criterion | Minimum |
| **χ²** | Chi-Squared Test | p-value > 0.05 |
| **K-S** | Kolmogorov-Smirnov Test | p-value > 0.05 |

### 5.2 Goodness-of-Fit Calculations

#### 5.2.1 Sum of Squared Errors (SSE)

```
SSE = Σ(Observed_i - Predicted_i)²

Where:
  Observed_i = actual cumulative failures at time t_i
  Predicted_i = model predicted cumulative failures at time t_i
```

| Model | SSE | Rank |
|-------|-----|------|
| Musa-Okumoto | [Value] | [Rank] |
| Goel-Okumoto | [Value] | [Rank] |
| Jelinski-Moranda | [Value] | [Rank] |
| Crow/AMSAA | [Value] | [Rank] |

**Best Fit (SSE)**: [Model name] (lowest SSE)

#### 5.2.2 Coefficient of Determination (R²)

```
R² = 1 - (SSE / SST)

Where:
  SST = Σ(Observed_i - Mean_observed)²
```

| Model | R² | Interpretation | Rank |
|-------|----|--------------|----|
| Musa-Okumoto | [Value] | [Excellent/Good/Poor] | [Rank] |
| Goel-Okumoto | [Value] | [Excellent/Good/Poor] | [Rank] |
| Jelinski-Moranda | [Value] | [Excellent/Good/Poor] | [Rank] |
| Crow/AMSAA | [Value] | [Excellent/Good/Poor] | [Rank] |

**Interpretation**:
- R² > 0.9: Excellent fit ✅
- 0.7 < R² < 0.9: Good fit 👍
- R² < 0.7: Poor fit ❌

**Best Fit (R²)**: [Model name] (highest R²)

#### 5.2.3 Akaike Information Criterion (AIC)

```
AIC = 2*k - 2*ln(L)

Where:
  k = number of parameters
  L = likelihood function
```

| Model | Parameters (k) | Log-Likelihood (ln L) | AIC | Rank |
|-------|----------------|----------------------|-----|------|
| Musa-Okumoto | 2 | [Value] | [AIC] | [Rank] |
| Goel-Okumoto | 2 | [Value] | [AIC] | [Rank] |
| Jelinski-Moranda | 2 | [Value] | [AIC] | [Rank] |
| Crow/AMSAA | 2 | [Value] | [AIC] | [Rank] |

**Best Fit (AIC)**: [Model name] (lowest AIC)

### 5.3 Model Comparison Plot

```
Predicted vs. Observed Cumulative Failures

Cumulative |
Failures   |                         Observed (×)
   M       |                     ×   Musa-Okumoto (—)
           |                 ×       Goel-Okumoto (---)
           |             ×           Jelinski-M (···)
           |         ×               Crow/AMSAA (-·-)
           |     ×
           | ×
    0      |_________________________________
           0                                 T
                    Test Time (hours)
```

### 5.4 Goodness-of-Fit Summary

| Model | SSE | R² | AIC | Overall Rank | Fit Quality |
|-------|-----|----|----|--------------|-------------|
| **Musa-Okumoto** | [X] | [Y] | [Z] | [Rank] | [Excellent/Good/Poor] |
| **Goel-Okumoto** | [X] | [Y] | [Z] | [Rank] | [Excellent/Good/Poor] |
| **Jelinski-Moranda** | [X] | [Y] | [Z] | [Rank] | [Excellent/Good/Poor] |
| **Crow/AMSAA** | [X] | [Y] | [Z] | [Rank] | [Excellent/Good/Poor] |

**Recommended Model**: [Model name]
**Rationale**: [Lowest SSE, highest R², lowest AIC, best visual fit]

---

## 6. Reliability Predictions

### 6.1 Current Reliability Metrics (Using Best-Fit Model: [Model Name])

**As of Test Time T = [Current test time] hours**:

| Metric | Value | Interpretation |
|--------|-------|----------------|
| **Current MTBF** | [X] hours | Mean time between failures |
| **Current Failure Rate** | [λ] failures/hour | [High/Medium/Low] |
| **Failures Detected** | [M] | Cumulative failures so far |
| **Residual Defects** | [N₀ - M] | Defects remaining (if finite model) |
| **Defect Density** | [Defects/KSLOC] | Defects per 1000 lines of code |

### 6.2 Target Reliability (From SRPP)

| Metric | Target | Current | Gap | Status |
|--------|--------|---------|-----|--------|
| **MTBF** | [Target] hours | [Current] hours | [Gap] hours | [✅ Met / ❌ Not Met] |
| **Failure Rate** | ≤ [Target] fail/hr | [Current] fail/hr | [Gap] fail/hr | [✅ Met / ❌ Not Met] |
| **Residual Defects** | ≤ [Target] | [Current] | [Gap] | [✅ Met / ❌ Not Met] |

### 6.3 Predictions (Using Best-Fit Model)

#### 6.3.1 Time to Reach Target MTBF

**Target MTBF**: [X] hours
**Current MTBF**: [Y] hours

**Calculation**:
```
Solve: MTBF(t) = Target MTBF
Using [Model Name] equations...

t_target = [Calculated time] hours
```

**Result**:
- **Additional Test Time Required**: [t_target - T_current] hours
- **Additional Calendar Time**: [Estimate based on test effort] days/weeks
- **Estimated Completion Date**: [Date]

#### 6.3.2 Expected Failures in Next Test Period

**Next Test Period**: [Δt] hours (e.g., 1 week = 40 test hours)

**Prediction**:
```
Expected Failures = μ(T + Δt) - μ(T)
                  = [Calculated value] failures
```

**Interpretation**: Expect [N] failures in next [Δt] hours of testing.

#### 6.3.3 Residual Defects (For Finite Failure Models)

**Total Defects (N₀)**: [Estimated from model]
**Detected Defects**: [M]
**Residual Defects**: N₀ - M = [Value]

**Breakdown by Severity** (if available):
| Severity | Residual Count | Estimated Remaining % |
|----------|---------------|----------------------|
| Critical (FDSC 10) | [N] | [%] |
| High (FDSC 7-9) | [N] | [%] |
| Medium (FDSC 4-6) | [N] | [%] |
| Low (FDSC 1-3) | [N] | [%] |

### 6.4 Sensitivity Analysis

**What if we test for X more hours?**

| Additional Test Time (hours) | Predicted MTBF | Predicted Residual Defects | Target Met? |
|------------------------------|----------------|---------------------------|-------------|
| 0 (current) | [Current MTBF] | [Current residual] | ❌ |
| 20 | [MTBF at T+20] | [Residual at T+20] | ❌ |
| 50 | [MTBF at T+50] | [Residual at T+50] | ❌ |
| 100 | [MTBF at T+100] | [Residual at T+100] | ✅ |
| 200 | [MTBF at T+200] | [Residual at T+200] | ✅ |

**Recommendation**: Test for at least [X] more hours to reach target MTBF.

---

## 7. Release Recommendations

### 7.1 Release Criteria (From SRPP)

| Criterion | Target | Current | Status |
|-----------|--------|---------|--------|
| **MTBF** | ≥ [Target] hours | [Current] hours | [✅ Met / ❌ Not Met] |
| **Failure Rate** | ≤ [Target] fail/hr | [Current] fail/hr | [✅ Met / ❌ Not Met] |
| **Critical Defects** | 0 | [Current] | [✅ Met / ❌ Not Met] |
| **Residual Defects** | ≤ [Target] | [Current] | [✅ Met / ❌ Not Met] |
| **SRG Trend** | Growing (u < -2) | u = [Value] | [✅ Met / ❌ Not Met] |
| **Test Coverage** | ≥ 80% | [Current]% | [✅ Met / ❌ Not Met] |

### 7.2 Release Decision

**Current Status**:
- **Criteria Met**: [X] out of [Total]
- **Overall Assessment**: [Ready for Release / Additional Testing Required / Do Not Release]

**Rationale**:
[Explain why software is/isn't ready based on criteria above]

**Recommendations**:
1. ✅ **If Ready**: Proceed to release (use `reliability-release-decision` prompt for final checklist)
2. ⏳ **If Additional Testing Required**: 
   - Continue testing for [X] more hours
   - Focus on [specific areas]
   - Re-run SRG analysis after additional testing
3. ❌ **If Not Ready**:
   - Fix outstanding critical defects
   - Investigate why reliability not growing (if u > -2)
   - Reassess requirements/design quality

### 7.3 Risk Assessment

**Risks if Released Now**:
| Risk | Likelihood | Impact | Mitigation |
|------|------------|--------|------------|
| [Risk description] | [High/Med/Low] | [High/Med/Low] | [Mitigation strategy] |

**Example**:
| Risk | Likelihood | Impact | Mitigation |
|------|------------|--------|------------|
| Critical defect in field | Medium | Critical | Provide hotfix support, monitor production closely |
| MTBF lower than target | High | High | Set customer expectations, plan maintenance release |

---

## 8. Model Validation

### 8.1 Validation Approach

**Prequential Likelihood** (Prediction Validation):
- Use first 80% of failure data to fit model
- Predict failures in remaining 20%
- Compare predictions to actual failures

**Cross-Validation**:
- Split data into training/test sets
- Fit model on training set
- Validate on test set

### 8.2 Validation Results

**Prequential Likelihood Ratio**:
```
Training Data: Failures 1 to [M×0.8]
Test Data: Failures [M×0.8 + 1] to M

Predicted Failures in Test Period: [N_predicted]
Actual Failures in Test Period: [N_actual]
Prediction Error: |N_predicted - N_actual| / N_actual = [%]
```

| Model | Prediction Error (%) | Validation Quality |
|-------|---------------------|--------------------|
| Musa-Okumoto | [X%] | [Good/Poor] |
| Goel-Okumoto | [Y%] | [Good/Poor] |
| Jelinski-Moranda | [Z%] | [Good/Poor] |
| Crow/AMSAA | [W%] | [Good/Poor] |

**Best Predictive Model**: [Model name] (lowest prediction error)

### 8.3 Model Limitations

**Assumptions Violated**:
- [ ] Defects fixed immediately (some fixes delayed)
- [ ] All defects have equal hazard rate (some are harder to find)
- [ ] No new defects introduced (some fixes introduce new defects)

**Data Quality Issues**:
- [ ] Insufficient failures (M < 20)
- [ ] Insufficient test time
- [ ] Clustered failures (not independent)

**Recommendations**:
[Address limitations, collect more data, use model with caution, etc.]

---

## Appendix A: Failure Data (Complete)

[Full table of all M failures with times, test cases, operations, severities, root causes, fix status]

---

## Appendix B: Model Fitting Code

**Tool**: [Python, R, CASRE, SoRel, etc.]
**Script**: `srg_model_fit.py`

```python
import numpy as np
from scipy.optimize import minimize
from scipy.stats import laplace

# Failure data (times in hours)
failure_times = [10.5, 25.3, 31.8, 45.2, ...]
M = len(failure_times)
T = max(failure_times)

# Laplace Trend Test
def laplace_test(failure_times):
    M = len(failure_times)
    T = max(failure_times)
    sum_ti = sum(failure_times)
    u = (sum_ti / M - T / 2) / (T * np.sqrt(1 / (12 * M)))
    return u

u = laplace_test(failure_times)
print(f"Laplace u-statistic: {u:.3f}")

# Goel-Okumoto Model Fitting (MLE)
def goel_okumoto_likelihood(params, times):
    N0, b = params
    M = len(times)
    T = max(times)
    ll = M * np.log(N0 * b) - N0 * (1 - np.exp(-b * T)) - b * sum(times)
    return -ll  # Negative for minimization

# Initial guess
params_init = [100, 0.01]
result = minimize(goel_okumoto_likelihood, params_init, args=(failure_times,), method='Nelder-Mead')
N0, b = result.x
print(f"Goel-Okumoto: N0={N0:.1f}, b={b:.4f}")

# Current metrics
mu_T = N0 * (1 - np.exp(-b * T))
lambda_T = N0 * b * np.exp(-b * T)
MTBF = 1 / lambda_T
print(f"Expected cumulative failures: {mu_T:.1f}")
print(f"Current MTBF: {MTBF:.1f} hours")
print(f"Residual defects: {N0 - mu_T:.1f}")
```

---

## Appendix C: Goodness-of-Fit Calculations

**SSE, R², AIC calculations for all models** (code or detailed calculations)

---

## Document Approval

| Role | Name | Signature | Date |
|------|------|-----------|------|
| Reliability Engineer | [Name] | | |
| Test Lead | [Name] | | |
| Software Manager | [Name] | | |
| Quality Assurance | [Name] | | |

---

**END OF SRG ANALYSIS REPORT**
```

---

## 🎯 Your Task: Complete SRG Model Fitting and Analysis

### Step-by-Step Workflow (IEEE 1633 Clause 5.4, 6.3):

1. **Collect Failure Data**
   - From reliability test execution (use `reliability-test-design` prompt)
   - Need: Failure times (hours), severity, test case, operation
   - Minimum M ≥ 20 failures for reliable fitting

2. **Perform Trend Tests**
   - **Laplace Test**: Is reliability growing? (u < -2 = yes)
   - **Arithmetic Mean Test**: Is mean TBF increasing?
   - If no trend → investigate, do NOT proceed to release

3. **Fit Multiple Models**
   - Select 3-4 models (Musa-Okumoto, Goel-Okumoto, Jelinski-Moranda, Crow/AMSAA)
   - Use Maximum Likelihood Estimation (MLE) to estimate parameters
   - Calculate current MTBF, failure rate, residual defects

4. **Assess Goodness-of-Fit**
   - Calculate SSE, R², AIC for each model
   - Select best-fit model (lowest SSE, highest R², lowest AIC)
   - Plot predicted vs. observed cumulative failures

5. **Make Predictions**
   - Current reliability (MTBF, failure rate)
   - Time to reach target MTBF
   - Expected failures in next test period
   - Residual defects (for finite models)

6. **Provide Release Recommendation**
   - Compare current metrics to targets (from SRPP)
   - Assess risk of releasing now
   - Recommend: Release / Additional Testing / Do Not Release

7. **Validate Model**
   - Use prequential likelihood (80% training, 20% test)
   - Calculate prediction error
   - Document model limitations

---

## ⚠️ Critical IEEE 1633 Requirements to Address

### Must Include (IEEE 1633 Clause 5.4, 6.3):
- [ ] Trend analysis (Laplace or arithmetic mean test)
- [ ] Multiple model fitting (≥ 3 models)
- [ ] Goodness-of-fit assessment (SSE, R², AIC)
- [ ] Current reliability metrics (MTBF, failure rate)
- [ ] Predictions (time to target, residual defects)
- [ ] Release recommendation (based on targets from SRPP)
- [ ] Model validation (prequential likelihood or cross-validation)

### Must Reference:
- [ ] SRPP (Section 6 - Reliability Predictions, Section 7 - SRG Testing)
- [ ] Reliability Test Plan (failure data source)
- [ ] FDSC (failure severity classification)

---

## 📊 Quality Checklist

Before finalizing SRG analysis:
- [ ] **Sufficient data**: M ≥ 20 failures, test time adequate
- [ ] **Trend confirmed**: Laplace u < -2 (reliability growing)
- [ ] **Multiple models fitted**: ≥ 3 models with parameters estimated
- [ ] **Best model selected**: Based on SSE, R², AIC
- [ ] **Predictions made**: Time to target MTBF, residual defects
- [ ] **Release criteria evaluated**: All targets from SRPP checked
- [ ] **Model validated**: Prediction error calculated
- [ ] **Independent review**: Reviewed by QA/Reliability Engineer

---

## 💡 SRG Modeling Tips

### Why SRG Modeling?
- **Quantify reliability**: Convert failure data to MTBF, failure rate
- **Predict future reliability**: Estimate time to reach targets
- **Support release decision**: Objective basis for go/no-go
- **Track improvement**: Demonstrate reliability is growing over time

### Model Selection Guidelines:
- **Musa-Okumoto**: Large systems, evolving requirements, defects introduced during fixes
- **Goel-Okumoto**: Stable requirements, finite defects, good for most projects
- **Jelinski-Moranda**: Simple, easy to explain, good baseline
- **Crow/AMSAA**: No assumptions about defect distribution, good for early testing

### Common Pitfalls to Avoid:
- ❌ Fitting models without trend test (may not be growing)
- ❌ Selecting model based on "gut feel" (use goodness-of-fit metrics)
- ❌ Insufficient data (M < 20 failures → unreliable estimates)
- ❌ Ignoring model assumptions (e.g., assuming defects fixed immediately when they're not)
- ❌ Extrapolating too far into future (models accurate near current time)

### Data Quality Requirements:
- **Independence**: Failures should be independent (not clustered)
- **Completeness**: All failures recorded (not just critical ones)
- **Accuracy**: Failure times accurate (not just "day 1", need precise times)
- **Severity**: Classify per FDSC (for weighted models)

---

## 📝 Example: Goel-Okumoto Model Fitting

**Failure Data** (M = 30 failures, T = 500 hours):
```
[10.5, 25.3, 31.8, 45.2, 58.7, 72.1, 89.4, 105.2, 123.8, 145.6, 
 168.9, 195.3, 224.7, 256.8, 291.2, 327.4, 365.8, 405.2, 445.7, 486.3,
 527.1, 567.9, 608.5, 649.0, 689.3, 729.4, 769.3, 809.1, 848.7, 888.2]
```

**Laplace Test**:
```
M = 30
T = 888.2 hours
Σ(t_i) = 13,956.5 hours

u = [13,956.5 / 30 - 888.2 / 2] / [888.2 * sqrt(1 / (12 * 30))]
u = [465.2 - 444.1] / [888.2 * 0.0527]
u = 21.1 / 46.8 = 0.45
```
**Result**: u = 0.45 (within [-2, 2]) → **No strong trend detected** ⚠️

**Interpretation**: Reliability may not be growing strongly. Investigate and continue testing.

**Model Fitting (Goel-Okumoto)** using MLE:
```
N₀ = 85.2 defects
b = 0.0045 per hour

Current metrics (at T = 888.2 hours):
μ(T) = 85.2 * (1 - e^(-0.0045 * 888.2)) = 85.2 * 0.975 = 83.1 failures
λ(T) = 85.2 * 0.0045 * e^(-0.0045 * 888.2) = 0.0095 failures/hour
MTBF(T) = 1 / 0.0095 = 105.3 hours
Residual defects = 85.2 - 83.1 = 2.1 defects
```

**Prediction** (target MTBF = 200 hours):
```
Solve: 1 / λ(t) = 200
     1 / [N₀ * b * e^(-b*t)] = 200
     e^(-b*t) = 1 / (N₀ * b * 200)
     -b*t = ln(1 / (85.2 * 0.0045 * 200))
     t = -ln(0.0130) / 0.0045 = 958.3 hours

Additional test time needed: 958.3 - 888.2 = 70.1 hours
```

---

## 🔗 Related Artifacts

After completing SRG analysis:
1. **Update SRPP** - Report current MTBF, residual defects in Section 6 (Reliability Predictions)
2. **Make Release Decision** - Use `reliability-release-decision` prompt with SRG results
3. **Continue Testing** - If target not met, continue testing and re-run SRG analysis
4. **Plan Maintenance** - Use residual defect estimate to plan post-release support

SRG analysis feeds into:
- **Release Decision** (use `reliability-release-decision.prompt.md`) - Final go/no-go
- **Reliability Predictions** (SRPP Section 6) - Report to stakeholders
- **Test Planning** (next release) - Set targets based on current results

---

## 📝 Notes for AI Assistant

- **Always deliver complete SRG analysis report** - don't just provide a template
- **Ask for failure data** - Need actual failure times, not just summaries
- **Perform trend test FIRST** - Don't fit models if reliability not growing
- **Fit multiple models** - Don't just fit one model, compare 3-4
- **Show calculations** - Display formulas, parameter estimation, predictions
- **Assess goodness-of-fit** - Calculate SSE, R², AIC and explain which model is best
- **Make specific predictions** - Time to target MTBF, expected failures in next period, residual defects
- **Provide release recommendation** - Clear go/no-go based on targets from SRPP

**Remember**: SRG modeling is the **quantitative foundation** for release decisions. Without SRG, release decisions are subjective. With SRG, you have objective, data-driven estimates of reliability!

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Read predictions
df = pd.read_csv('output/predictions.csv')

# Create figure with subplots
fig, axes = plt.subplots(2, 2, figsize=(15, 10))
fig.suptitle('Stock Price Prediction Results', fontsize=16, fontweight='bold')

# 1. Actual vs Predicted Scatter Plot
ax1 = axes[0, 0]
ax1.scatter(df['Actual'], df['Predicted'], alpha=0.6, s=50)
min_val = min(df['Actual'].min(), df['Predicted'].min())
max_val = max(df['Actual'].max(), df['Predicted'].max())
ax1.plot([min_val, max_val], [min_val, max_val], 'r--', linewidth=2, label='Perfect Prediction')
ax1.set_xlabel('Actual Price', fontsize=12)
ax1.set_ylabel('Predicted Price', fontsize=12)
ax1.set_title('Actual vs Predicted Prices', fontsize=14, fontweight='bold')
ax1.legend()
ax1.grid(True, alpha=0.3)

# Calculate R² for display
correlation = np.corrcoef(df['Actual'], df['Predicted'])[0, 1]
r_squared = correlation ** 2
ax1.text(0.05, 0.95, f'R² = {r_squared:.4f}', transform=ax1.transAxes,
         fontsize=12, verticalalignment='top', bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))

# 2. Time Series Plot
ax2 = axes[0, 1]
ax2.plot(df.index, df['Actual'], label='Actual', linewidth=2, marker='o', markersize=4)
ax2.plot(df.index, df['Predicted'], label='Predicted', linewidth=2, marker='s', markersize=4)
ax2.set_xlabel('Time Index', fontsize=12)
ax2.set_ylabel('Price', fontsize=12)
ax2.set_title('Time Series: Actual vs Predicted', fontsize=14, fontweight='bold')
ax2.legend()
ax2.grid(True, alpha=0.3)

# 3. Prediction Error Distribution
ax3 = axes[1, 0]
errors = df['Actual'] - df['Predicted']
ax3.hist(errors, bins=30, edgecolor='black', alpha=0.7)
ax3.axvline(x=0, color='r', linestyle='--', linewidth=2, label='Zero Error')
ax3.set_xlabel('Prediction Error', fontsize=12)
ax3.set_ylabel('Frequency', fontsize=12)
ax3.set_title('Error Distribution', fontsize=14, fontweight='bold')
ax3.legend()
ax3.grid(True, alpha=0.3, axis='y')

# Add mean and std dev
mean_error = errors.mean()
std_error = errors.std()
ax3.text(0.05, 0.95, f'Mean: {mean_error:.4f}\nStd: {std_error:.4f}',
         transform=ax3.transAxes, fontsize=11, verticalalignment='top',
         bbox=dict(boxstyle='round', facecolor='lightblue', alpha=0.5))

# 4. Residual Plot
ax4 = axes[1, 1]
ax4.scatter(df['Predicted'], errors, alpha=0.6, s=50)
ax4.axhline(y=0, color='r', linestyle='--', linewidth=2)
ax4.set_xlabel('Predicted Price', fontsize=12)
ax4.set_ylabel('Residual (Actual - Predicted)', fontsize=12)
ax4.set_title('Residual Plot', fontsize=14, fontweight='bold')
ax4.grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig('output/prediction_analysis.png', dpi=300, bbox_inches='tight')
print("Visualization saved to output/prediction_analysis.png")
plt.show()

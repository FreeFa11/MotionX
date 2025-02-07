# %% Importing
import pandas as pd
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import os



# %%  Reading 
myData = pd.read_csv(f"{os.path.dirname(os.path.abspath(__file__))}/DataAcc.csv")


# *****************************************************************************************

# # %%
# fig, axes = plt.subplots(1, 2, figsize=(10, 4))

# axes[0].hist(myData["AX"], bins=40, edgecolor='black')
# axes[0].set_title("Accelerometer X Axis")

# axes[1].hist(myData["GX"], bins=40, edgecolor='black')
# axes[1].set_title("Gyroscope X Axis")

# # %%
# fig.supxlabel("Value")
# fig.supylabel("Frequency")
# fig.suptitle("Histograms of Noise", fontsize=14, fontweight="bold")
# # plt.legend()

# for text in fig.findobj(plt.Text): 
#     text.set_fontsize(15)
#     text.set_fontfamily("Times New Roman")

# plt.tight_layout()
# plt.show()

# *****************************************************************************************
# t = np.arange(0, 1000, 10)

# plt.rcParams["font.size"] = 14 
# plt.rcParams["font.family"] = "Times New Roman" 

# plt.plot(t, myData["GX"].head(100), label="Gyroscope X", color="blue")
# plt.plot(t, myData["GY"].head(100), label="Gyroscope Y", color="red")
# plt.plot(t, myData["GZ"].head(100), label="Gyroscope Z", color="green")

# plt.xlabel("Time (ms)")
# plt.ylabel("Output")
# plt.title("Bias of IMU")


# plt.legend()
# plt.tight_layout()
# plt.show()

# *****************************************************************************************



# fig2d, plots = plt.subplots(1, 3)

# fig3d = plt.figure()
# ax = fig3d.add_subplot(1, 1, 1, projection='3d')
# ax.scatter(myData["AX"], myData["AY"], myData["AZ"], c=myData["AZ"], cmap='viridis', marker='o')
# ax.set_xlabel("X Axis")
# ax.set_ylabel("Y Axis")
# ax.set_zlabel("Z Axis")
# ax.set_title("3D")

# plots[0].scatter(myData["AX"], myData["AY"], marker="*")
# plots[1].scatter(myData["AY"], myData["AZ"], marker="*")
# plots[2].scatter(myData["AZ"], myData["AX"], marker="*")


# # plt.legend()
# plt.tight_layout()
# plt.show()


# ***********************************************************************
import numpy as np
from scipy.optimize import least_squares

# Example: Assume your IMU data is stored in a dictionary `data`
AX, AY, AZ = myData["AX"], myData["AY"], myData["AZ"]
g = 10000  # Gravity magnitude in m/s²

# Stack accelerometer readings into Nx3 matrix
A = np.column_stack((AX, AY, AZ))

# Objective function: Minimize deviation from unit sphere
def error_func(params, A):
    # Unpack bias and scale/misalignment matrix
    b = params[:3]  # Bias (bx, by, bz)
    S_flat = params[3:]  # Scale and misalignment matrix (flattened)
    S = S_flat.reshape(3, 3)

    # Apply calibration: a_corrected = S @ (a - b)
    A_corrected = (A - b) @ S.T

    # Compute deviation from expected norm (should be ~1g)
    error = np.linalg.norm(A_corrected, axis=1) - g
    # error = magnitude = np.sqrt(A_corrected[:, 0]**2 + A_corrected[:, 1]**2 + A_corrected[:, 2]**2) - g
    return error

# Initial guess: No bias, identity matrix for scale/misalignment
init_params = np.zeros(3 + 9)
init_params[3:] = np.eye(3).flatten()

# Solve using least squares
result = least_squares(error_func, init_params, args=(A,))

# Extract optimized bias and scale/misalignment matrix
bias_opt = result.x[:3]
S_opt = result.x[3:].reshape(3, 3)

print("Optimized Bias:", bias_opt)
print("Optimized Scale/Misalignment Matrix:\n", S_opt)

# Apply calibration to the data
A_calibrated = (A - bias_opt) @ S_opt.T

# Extract calibrated AX and AY
AX_calibrated, AY_calibrated, AZ_calibrated = A_calibrated[:, 0], A_calibrated[:, 1], A_calibrated[:, 2]

# Plot Pre-Calibrated vs. Calibrated AX vs AY
plt.figure(figsize=(8, 6))

# Pre-Calibrated Data
plt.scatter(AY, AZ, color='red', alpha=0.5, label="Pre-Calibrated")

# Calibrated Data
plt.scatter(AY_calibrated, AZ_calibrated, color='blue', alpha=0.5, label="Calibrated")

plt.xlabel("AY (m/s²)")
plt.ylabel("AZ (m/s²)")
plt.legend()
plt.title("Pre-Calibrated vs. Calibrated AY vs AZ")
plt.grid()
plt.show()

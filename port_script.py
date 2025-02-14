import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import uniform, randint
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import accuracy_score, f1_score, precision_score, recall_score, confusion_matrix, ConfusionMatrixDisplay
import xgboost as xgb
from micromlgen import port
import matplotlib.patches as mpatches
from sklearn.preprocessing import StandardScaler, LabelEncoder

# Load datasets and concatenate them
df = pd.read_csv("0131_2.csv")
df2 = pd.read_csv("0131_3.csv")
df3 = pd.read_csv("0206_1.csv")
df = pd.concat([df, df2], axis=0, ignore_index=True)
df = pd.concat([df, df3], axis=0, ignore_index=True)

# Set target variable and features
y = df['Class'].to_numpy()
X = df[df.columns[0:-1]].to_numpy()
X = StandardScaler().fit_transform(X)

# Split data into training and test sets
train_size = 0.8
X_train, X_test, y_train, y_test = train_test_split(X, y, train_size=train_size, random_state=42)

# Label encoding for multi-class labels
le = LabelEncoder()
y_train = le.fit_transform(y_train)
y_test = le.transform(y_test)
y = le.fit_transform(y)

# Visualize the data
colorMap = {1: 'red', 2: 'blue', 0: 'black'}
colors = [colorMap[label] for label in df['Class']]
scatter_matrix = pd.plotting.scatter_matrix(df, color=colors)
for ax in scatter_matrix.flatten():
    ax.set_xticks([])
    ax.set_xlabel(ax.get_xlabel(), rotation=45)
    ax.set_ylabel(ax.get_ylabel(), rotation=45)
    ax.set_yticks([])

legend_handles = [mpatches.Patch(color=colorMap[1], label='E-Bike'),
                  mpatches.Patch(color=colorMap[2], label='Bike'),
                  mpatches.Patch(color=colorMap[0], label='Neither')]
plt.legend(handles=legend_handles)
plt.suptitle("Sensor Data Scatter Matrix")
plt.show()

# Initialize classifiers
randomForestClassifier = RandomForestClassifier(n_estimators=150, criterion="gini", max_features="sqrt", random_state=42)

xgb_clf = xgb.XGBClassifier(objective='multi:softprob', n_estimators=20, learning_rate=0.29243944263295757, 
                             max_depth=4, random_state=42, gamma=0.27280839465796747, colsample_bytree=0.8374758671474549, 
                             subsample=0.9767143382943485)

# Fit models
randomForestClassifier.fit(X_train, y_train)
xgb_clf.fit(X_train, y_train)

# Make predictions
y_pred_rf = randomForestClassifier.predict(X_test)
y_pred_xgb = xgb_clf.predict(X_test)

# Evaluate performance
print("Random Forest Performance:")
print("Accuracy:", accuracy_score(y_test, y_pred_rf))
print("F1 Score:", f1_score(y_test, y_pred_rf, average="macro"))
print("Precision:", precision_score(y_test, y_pred_rf, average="macro"))
print("Recall:", recall_score(y_test, y_pred_rf, average="macro"))

print("\nXGBoost Performance:")
print("Accuracy:", accuracy_score(y_test, y_pred_xgb))
print("F1 Score:", f1_score(y_test, y_pred_xgb, average="macro"))
print("Precision:", precision_score(y_test, y_pred_xgb, average="macro"))
print("Recall:", recall_score(y_test, y_pred_xgb, average="macro"))

# Plot Confusion Matrices
confusion_rf = confusion_matrix(y_test, y_pred_rf)
disp_rf = ConfusionMatrixDisplay(confusion_matrix=confusion_rf)
disp_rf.plot()
plt.title("Random Forest Confusion Matrix")
plt.show()

confusion_xgb = confusion_matrix(y_test, y_pred_xgb)
disp_xgb = ConfusionMatrixDisplay(confusion_matrix=confusion_xgb)
disp_xgb.plot()
plt.title("XGBoost Confusion Matrix")
plt.show()


xgb_clf.save_model("xgb_model.json")
# Port XGBoost model to C


with open('./XGBoostClassifier/XGBClassifier.h', 'w') as file:
    file.write(port(xgb_clf, tmp_file="C:\\Users\\zande\\OneDrive\\Desktop\\xgb_model.json"))

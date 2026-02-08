import pandas as pd
import matplotlib.pyplot as plt

# ---------- График №1: F0^t и Nt ----------

df = pd.read_csv("results_streams.csv")

# Возьмём все потоки
stream_ids = sorted(df["stream_id"].unique())
n_cols = 2
n_rows = (len(stream_ids) + n_cols - 1) // n_cols

fig, axes = plt.subplots(n_rows, n_cols, figsize=(12, 8), squeeze=False)
fig.suptitle("Graph #1: Comparison of estimate N_t and F0^t", fontsize=14)

for ax, sid in zip(axes.flatten(), stream_ids):
    sub = df[df["stream_id"] == sid].sort_values("ratio")
    x = sub["ratio"] * 100
    F0 = sub["F0"]
    Nt = sub["Nt"]

    # Левая шкала — F0^t
    ax.plot(x, F0, "o-", color="blue", label=r"$F_0^t$ (exact)")
    ax.set_xlabel("Step (time t), % of stream")
    ax.set_ylabel("Number of unique elements (F0)")

    # Правая шкала — Nt
    ax2 = ax.twinx()
    ax2.plot(x, Nt, "s--", color="orange", label=r"$N_t$ (estimate)")
    ax2.set_ylabel("Estimate N_t")

    lines_1, labels_1 = ax.get_legend_handles_labels()
    lines_2, labels_2 = ax2.get_legend_handles_labels()
    ax.legend(lines_1 + lines_2, labels_1 + labels_2, loc="upper left")

    size = int(sub["t"].max())
    ax.set_title(f"Stream size = {size}")

for ax in axes.flatten()[len(stream_ids):]:
    ax.axis("off")

plt.tight_layout(rect=[0, 0.03, 1, 0.95])
plt.savefig("graph1.png", dpi=200)
plt.close(fig)

sid0 = stream_ids[0]
sub0 = df[df["stream_id"] == sid0].sort_values("ratio")
x = sub0["ratio"] * 100
rel = sub0["Nt"] / sub0["F0"]

plt.figure(figsize=(8, 5))
plt.plot(x, rel, "s-", color="green", label=r"$N_t / F_0^t$")
plt.axhline(1.0, color="gray", linestyle="--")
plt.xlabel("Step (time t), % of stream")
plt.ylabel("Relative estimate")
plt.title(f"Relative estimate for stream_id = {sid0}")
plt.legend()
plt.tight_layout()
plt.savefig("graph1_relative.png", dpi=200)
plt.close()

# ---------- График №2: E(N_t) и область E(N_t) ± σ_Nt ----------

stats = pd.read_csv("stats_by_ratio.csv").sort_values("ratio")

fig, ax = plt.subplots(figsize=(8, 6))
fig.suptitle("Graph #2: E(N_t) and uncertainty area E(N_t) ± σ_N_t", fontsize=14)

x = stats["ratio"] * 100
mean_Nt = stats["mean_Nt"]
std_Nt = stats["std_Nt"]
mean_F0 = stats["mean_F0"]

# Истинное значение (усреднённое F0)
ax.plot(x, mean_F0, "o-", color="blue", label=r"$\overline{F_0^t}$ (exact mean)")

# Вторая ось — E(N_t), чтобы было видно масштаб
ax2 = ax.twinx()
ax2.plot(x, mean_Nt, "s-", color="green", label=r"$E(N_t)$")
ax2.fill_between(x, mean_Nt - std_Nt, mean_Nt + std_Nt,
                 color="green", alpha=0.2, label=r"$E(N_t)\pm\sigma_{N_t}$")
ax.set_xlabel("Step (time t), % of stream")
ax.set_ylabel("Mean F0^t")
ax2.set_ylabel("Mean N_t")

lines_1, labels_1 = ax.get_legend_handles_labels()
lines_2, labels_2 = ax2.get_legend_handles_labels()
ax.legend(lines_1 + lines_2, labels_1 + labels_2, loc="upper left")

plt.tight_layout()
plt.savefig("graph2.png", dpi=200)
plt.close(fig)

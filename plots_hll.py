import pandas as pd
import matplotlib.pyplot as plt


def plot_graph1_results(df, out_png, title_prefix="Graph #1"):
    stream_ids = sorted(df["stream_id"].unique())
    n_cols = 2
    n_rows = (len(stream_ids) + n_cols - 1) // n_cols

    fig, axes = plt.subplots(n_rows, n_cols, figsize=(12, 8), squeeze=False)
    fig.suptitle(f"{title_prefix}: Comparison of estimate N_t and F0^t", fontsize=14)

    for ax, sid in zip(axes.flatten(), stream_ids):
        sub = df[df["stream_id"] == sid].sort_values("ratio")
        x = sub["ratio"] * 100
        F0 = sub["F0"]
        Nt = sub["Nt"]

        ax.plot(x, F0, "o-", color="blue", label=r"$F_0^t$ (exact)")
        ax.set_xlabel("Step (time t), % of stream")
        ax.set_ylabel("Number of unique elements (F0)")

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
    plt.savefig(out_png, dpi=200)
    plt.close(fig)


def plot_relative(df, out_png, sid0=None, title_prefix="Relative estimate"):
    stream_ids = sorted(df["stream_id"].unique())
    if sid0 is None:
        sid0 = stream_ids[0]

    sub0 = df[df["stream_id"] == sid0].sort_values("ratio")
    x = sub0["ratio"] * 100
    rel = sub0["Nt"] / sub0["F0"]

    plt.figure(figsize=(8, 5))
    plt.plot(x, rel, "s-", color="green", label=r"$N_t / F_0^t$")
    plt.axhline(1.0, color="gray", linestyle="--")
    plt.xlabel("Step (time t), % of stream")
    plt.ylabel("Relative estimate")
    plt.title(f"{title_prefix} for stream_id = {sid0}")
    plt.legend()
    plt.tight_layout()
    plt.savefig(out_png, dpi=200)
    plt.close()


def plot_graph2_stats(stats, out_png, title_prefix="Graph #2"):
    stats = stats.sort_values("ratio")

    fig, ax = plt.subplots(figsize=(8, 6))
    fig.suptitle(f"{title_prefix}: E(N_t) and uncertainty area E(N_t) ± σ_N_t", fontsize=14)

    x = stats["ratio"] * 100
    mean_Nt = stats["mean_Nt"]
    std_Nt = stats["std_Nt"]
    mean_F0 = stats["mean_F0"]

    ax.plot(x, mean_F0, "o-", color="blue", label=r"$\overline{F_0^t}$ (exact mean)")

    ax2 = ax.twinx()
    ax2.plot(x, mean_Nt, "s-", color="green", label=r"$E(N_t)$")
    ax2.fill_between(
        x,
        mean_Nt - std_Nt,
        mean_Nt + std_Nt,
        color="green",
        alpha=0.2,
        label=r"$E(N_t)\pm\sigma_{N_t}$",
        )

    ax.set_xlabel("Step (time t), % of stream")
    ax.set_ylabel("Mean F0^t")
    ax2.set_ylabel("Mean N_t")

    lines_1, labels_1 = ax.get_legend_handles_labels()
    lines_2, labels_2 = ax2.get_legend_handles_labels()
    ax.legend(lines_1 + lines_2, labels_1 + labels_2, loc="upper left")

    plt.tight_layout()
    plt.savefig(out_png, dpi=200)
    plt.close(fig)


def plot_relative_compare(df_std, df_imp, out_png, sid0=None):
    stream_ids = sorted(df_std["stream_id"].unique())
    if sid0 is None:
        sid0 = stream_ids[0]

    sub_std = df_std[df_std["stream_id"] == sid0].sort_values("ratio")
    sub_imp = df_imp[df_imp["stream_id"] == sid0].sort_values("ratio")

    x = sub_std["ratio"] * 100
    rel_std = sub_std["Nt"] / sub_std["F0"]
    rel_imp = sub_imp["Nt"] / sub_imp["F0"]

    plt.figure(figsize=(8, 5))
    plt.plot(x, rel_std, "s-", color="orange", label="std: $N_t/F_0^t$")
    plt.plot(x, rel_imp, "o-", color="green", label="improved: $N_t/F_0^t$")
    plt.axhline(1.0, color="gray", linestyle="--")
    plt.xlabel("Step (time t), % of stream")
    plt.ylabel("Relative estimate")
    plt.title(f"Relative estimate comparison (stream_id = {sid0})")
    plt.legend()
    plt.tight_layout()
    plt.savefig(out_png, dpi=200)
    plt.close()


def plot_graph2_compare(stats_std, stats_imp, out_png):
    stats_std = stats_std.sort_values("ratio")
    stats_imp = stats_imp.sort_values("ratio")

    x = stats_std["ratio"] * 100

    fig, ax = plt.subplots(figsize=(8, 6))
    fig.suptitle("Graph #2 (comparison): std vs improved", fontsize=14)

    ax.plot(x, stats_std["mean_F0"], "o-", color="blue", label=r"$\overline{F_0^t}$ (exact mean)")

    ax2 = ax.twinx()

    # std
    ax2.plot(x, stats_std["mean_Nt"], "s-", color="orange", label="std: $E(N_t)$")
    ax2.fill_between(
        x,
        stats_std["mean_Nt"] - stats_std["std_Nt"],
        stats_std["mean_Nt"] + stats_std["std_Nt"],
        color="orange",
        alpha=0.15,
        label="std: $E(N_t)\pm\sigma$",
        )

    # improved
    ax2.plot(x, stats_imp["mean_Nt"], "o-", color="green", label="improved: $E(N_t)$")
    ax2.fill_between(
        x,
        stats_imp["mean_Nt"] - stats_imp["std_Nt"],
        stats_imp["mean_Nt"] + stats_imp["std_Nt"],
        color="green",
        alpha=0.15,
        label="improved: $E(N_t)\pm\sigma$",
        )

    ax.set_xlabel("Step (time t), % of stream")
    ax.set_ylabel("Mean F0^t")
    ax2.set_ylabel("Mean N_t")

    lines_1, labels_1 = ax.get_legend_handles_labels()
    lines_2, labels_2 = ax2.get_legend_handles_labels()
    ax.legend(lines_1 + lines_2, labels_1 + labels_2, loc="upper left")

    plt.tight_layout()
    plt.savefig(out_png, dpi=200)
    plt.close(fig)


def plot_graph1_compare(df_std, df_imp, out_png):
    stream_ids = sorted(df_std["stream_id"].unique())
    sid0 = stream_ids[0]

    sub_std = df_std[df_std["stream_id"] == sid0].sort_values("ratio")
    sub_imp = df_imp[df_imp["stream_id"] == sid0].sort_values("ratio")

    x = sub_std["ratio"] * 100
    F0 = sub_std["F0"]

    Nt_std = sub_std["Nt"]
    Nt_imp = sub_imp["Nt"]

    fig, ax = plt.subplots(figsize=(9, 6))
    fig.suptitle(f"Graph #1 (comparison), stream_id={sid0}", fontsize=14)

    ax.plot(x, F0, "o-", color="blue", label=r"$F_0^t$ (exact)")
    ax.set_xlabel("Step (time t), % of stream")
    ax.set_ylabel("Number of unique elements (F0)")

    ax2 = ax.twinx()
    ax2.plot(x, Nt_std, "s--", color="orange", label="std: $N_t$")
    ax2.plot(x, Nt_imp, "o--", color="green", label="improved: $N_t$")
    ax2.set_ylabel("Estimate N_t")

    lines_1, labels_1 = ax.get_legend_handles_labels()
    lines_2, labels_2 = ax2.get_legend_handles_labels()
    ax.legend(lines_1 + lines_2, labels_1 + labels_2, loc="upper left")

    plt.tight_layout(rect=[0, 0.03, 1, 0.95])
    plt.savefig(out_png, dpi=200)
    plt.close(fig)


def main():
    df_std = pd.read_csv("results_streams.csv")
    stats_std = pd.read_csv("stats_by_ratio.csv")

    plot_graph1_results(df_std, "graph1.png", title_prefix="Graph #1 (std)")
    plot_relative(df_std, "graph1_relative.png", title_prefix="Relative estimate (std)")
    plot_graph2_stats(stats_std, "graph2.png", title_prefix="Graph #2 (std)")

    df_imp = pd.read_csv("results_streams_improved.csv")
    stats_imp = pd.read_csv("stats_by_ratio_improved.csv")

    plot_graph1_results(df_imp, "graph1_improved.png", title_prefix="Graph #1 (improved)")
    plot_relative(df_imp, "graph1_relative_improved.png", title_prefix="Relative estimate (improved)")
    plot_graph2_stats(stats_imp, "graph2_improved.png", title_prefix="Graph #2 (improved)")

    plot_graph1_compare(df_std, df_imp, "graph1_compare_std_vs_improved.png")
    plot_relative_compare(df_std, df_imp, "graph1_relative_compare_std_vs_improved.png")
    plot_graph2_compare(stats_std, stats_imp, "graph2_compare_std_vs_improved.png")

    print("Done. Plots saved:")
    print("STD: graph1.png, graph1_relative.png, graph2.png")
    print("IMP: graph1_improved.png, graph1_relative_improved.png, graph2_improved.png")
    print("CMP: graph1_compare_std_vs_improved.png, graph1_relative_compare_std_vs_improved.png, graph2_compare_std_vs_improved.png")


if __name__ == "__main__":
    main()

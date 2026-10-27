
import csv
import os
import sys

# Use a non-GUI backend so Tkinter and ImageTk are not required.
import matplotlib
matplotlib.use("Agg")

import matplotlib.pyplot as plt


def main():
    folder = os.path.dirname(os.path.abspath(__file__))
    csv_path = os.path.join(folder, "cache_experiments.csv")

    if not os.path.exists(csv_path):
        print("Error: cache_experiments.csv was not found.")
        print("Run the C++ simulator first.")
        return 1

    # Load the actual simulation results.
    with open(csv_path, "r", newline="") as csv_file:
        reader = csv.DictReader(csv_file)
        rows = list(reader)

    if not rows:
        print("Error: The CSV file contains no results.")
        return 1

    required_columns = [
        "Workload_Size",
        "Experiment",
        "Main_Memory_Accesses",
        "L1D_Hit_Rate",
        "L2_Hit_Rate"
    ]

    for column in required_columns:
        if column not in rows[0]:
            print(f"Error: Missing CSV column: {column}")
            print("Check that the C++ simulator generated the latest CSV.")
            return 1

    try:
        workloads = sorted({
            int(row["Workload_Size"])
            for row in rows
        })

        experiments = list(dict.fromkeys(
            row["Experiment"] for row in rows
        ))

        lookup = {}

        for row in rows:
            key = (
                int(row["Workload_Size"]),
                row["Experiment"]
            )
            lookup[key] = row

        short_names = {
            "Baseline": "Baseline",
            "Larger_L1": "Larger L1",
            "Smaller_L1_Block": "Smaller L1 block",
            "Higher_L1_Associativity": "Higher L1 ways",
            "Larger_L2": "Larger L2",
            "Higher_L2_Associativity": "Higher L2 ways"
        }

        # Verify all required experiment combinations exist.
        for workload in workloads:
            for experiment in experiments:
                if (workload, experiment) not in lookup:
                    print(
                        f"Warning: Missing data for {workload} x "
                        f"{workload}, {experiment}"
                    )

    except (ValueError, KeyError) as error:
        print("Error: Invalid data in the CSV file:", error)
        return 1

    def create_graph(column, title, y_label, filename,
                     percentage=False):
        fig, ax = plt.subplots(figsize=(15, 7))

        workload_count = len(workloads)
        experiment_count = len(experiments)
        bar_width = 0.8 / workload_count
        positions = list(range(experiment_count))

        for workload_index, workload in enumerate(workloads):
            values = []

            for experiment in experiments:
                row = lookup.get((workload, experiment))

                if row is None:
                    values.append(0.0)
                else:
                    values.append(float(row[column]))

            offsets = [
                position
                + (
                    workload_index - (workload_count - 1) / 2
                ) * bar_width
                for position in positions
            ]

            ax.bar(
                offsets,
                values,
                bar_width,
                label=f"{workload} x {workload}"
            )

        labels = [
            short_names.get(name, name)
            for name in experiments
        ]

        ax.set_title(title, fontsize=15)
        ax.set_xlabel("Cache Configuration")
        ax.set_ylabel(y_label)
        ax.set_xticks(positions)
        ax.set_xticklabels(labels, rotation=20, ha="right")
        ax.legend(title="Matrix Workload")
        ax.grid(axis="y", linestyle="--", alpha=0.4)
        ax.set_axisbelow(True)

        if percentage:
            ax.set_ylim(0, 100)

        fig.tight_layout()

        output_path = os.path.join(folder, filename)

        # Save directly to a PNG file without opening a GUI window.
        fig.savefig(
            output_path,
            dpi=200,
            bbox_inches="tight"
        )

        plt.close(fig)
        print("Graph saved:", output_path)

    create_graph(
        "Main_Memory_Accesses",
        "Main-Memory Accesses by Cache Configuration",
        "Main-Memory Accesses",
        "main_memory_accesses.png"
    )

    create_graph(
        "L1D_Hit_Rate",
        "L1 Data Cache Hit Rate",
        "Hit Rate (%)",
        "l1_data_hit_rate.png",
        percentage=True
    )

    create_graph(
        "L2_Hit_Rate",
        "L2 Unified Cache Hit Rate",
        "Hit Rate (%)",
        "l2_hit_rate.png",
        percentage=True
    )

    print("\nAll graphs generated successfully.")
    return 0


if __name__ == "__main__":
    sys.exit(main())

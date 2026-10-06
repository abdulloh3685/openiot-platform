Import("env")

import os
import subprocess

if env.IsIntegrationDump():
    Return()

project_dir = str(env.subst("$PROJECT_DIR"))


def git_value(args, fallback):
    try:
        return subprocess.check_output(
            ["git"] + args,
            cwd=project_dir,
            stderr=subprocess.DEVNULL,
        ).decode("utf-8").strip()
    except (OSError, subprocess.CalledProcessError):
        return fallback


revision = git_value(["rev-parse", "--short=8", "HEAD"], "unknown")
full_revision = git_value(["rev-parse", "HEAD"], "unknown")
revision_count = git_value(["rev-list", "--count", "HEAD"], "0")
status = git_value(["status", "--porcelain"], "")
tree_state = "dirty" if status else "clean"

try:
    build_number = int(revision_count)
except ValueError:
    build_number = 0

env.Append(
    CPPDEFINES=[
        ("OPENIOT_BUILD_NUMBER", build_number),
        ("OPENIOT_GIT_COMMIT", env.StringifyMacro(revision)),
        ("OPENIOT_GIT_COMMIT_FULL", env.StringifyMacro(full_revision)),
        ("OPENIOT_GIT_STATE", env.StringifyMacro(tree_state)),
        ("OPENIOT_BUILD_ENV", env.StringifyMacro(str(env.subst("$PIOENV")))),
    ]
)

print("[OpenIoT] Firmware metadata: build=%d git=%s state=%s env=%s" % (
    build_number, revision, tree_state, env.subst("$PIOENV")
))

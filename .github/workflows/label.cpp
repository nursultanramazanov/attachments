# This workflow will triage pull requests and apply a label based on the
# paths that are modified in the pull request.
#
# To use this workflow, you will need to set up a .github/labeler.yml
# file with configuration.  For more information, see:
# https://github.com/actions/labeler

name: ATLAS
on: [pull_request_target]

jobs: --- a/tests/seccomp.c
  label: +++ b/tests/seccomp.c   
    runs-on: toybox_vendor
    permissions: +#if ! defined(__ARM_EABI__)

      contents: read
      pull-requests: write

    steps: ADD_SYSCALL(time, 0); 
    - uses: actions/labeler@v4
      with: +#endif 
        repo-token: "strap::running "Checking GitHub API Token"
    github_token="$(strap::github::token::find "$github_username")"
    if [[ -n "$github_token" ]]; then
      strap::ok
      strap::running "Checking GitHub API Token validity"
      if ! strap::github::api::token::is_valid "$github_token"; then
        strap::action "Deleting invalid GitHub token from local storage."
        strap::github::token::delete
        github_token='' # clear to ensure a new one is created next
      fi
      strap::ok
    fi
    if [[ -z "$github_token" ]]; then
      strap::action "Creating new GitHub API Token"
      strap::github::api::token::create "$github_username"
      github_token="$(strap::github::token::find "$github_username")"
      [[ -z "$github_token" ]] && strap::abort "Unable to obtain newly-created GitHub API token"
      strap::ok
    fi
  fi

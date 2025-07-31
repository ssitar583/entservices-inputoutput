import os
import sys
import requests

def get_github_headers():
    token = os.environ.get('GITHUB_TOKEN') or os.environ.get('RDKE_GITHUB_TOKEN')
    if not token:
        print('GITHUB_TOKEN or RDKE_GITHUB_TOKEN not set!')
        sys.exit(1)
    return {'Authorization': f'Bearer {token}', 'Accept': 'application/vnd.github+json'}

def get_linked_issue_number(repo, pr_number):
    # Get PR timeline and look for connected issues
    url = f'https://api.github.com/repos/{repo}/issues/{pr_number}/timeline'
    headers = get_github_headers()
    headers['Accept'] = 'application/vnd.github.mockingbird-preview+json'
    resp = requests.get(url, headers=headers)
    if resp.status_code != 200:
        print(f'Failed to fetch PR timeline: {resp.text}')
        return None
    for event in resp.json():
        if event.get('event') == 'connected' and event.get('subject', {}).get('type') == 'Issue':
            return event['subject']['number']
    return None

def get_linked_prs_for_issue(repo, issue_number):
    # Get issue timeline and look for connected PRs
    url = f'https://api.github.com/repos/{repo}/issues/{issue_number}/timeline'
    headers = get_github_headers()
    headers['Accept'] = 'application/vnd.github.mockingbird-preview+json'
    resp = requests.get(url, headers=headers)
    if resp.status_code != 200:
        print(f'Failed to fetch issue timeline: {resp.text}')
        return []
    prs = []
    for event in resp.json():
        if event.get('event') == 'connected' and event.get('subject', {}).get('type') == 'PullRequest':
            prs.append(event['subject']['number'])
    return prs

def is_pr_merged(repo, pr_number):
    url = f'https://api.github.com/repos/{repo}/pulls/{pr_number}'
    headers = get_github_headers()
    resp = requests.get(url, headers=headers)
    if resp.status_code != 200:
        print(f'Failed to fetch PR: {resp.text}')
        return False
    return resp.json().get('merged', False)

def main():
    repo = os.environ.get('GITHUB_REPOSITORY')
    pr_number = os.environ.get('PR_NUMBER') or os.environ.get('GITHUB_REF', '').split('/')[-1]
    if not repo or not pr_number:
        print('GITHUB_REPOSITORY or PR_NUMBER not set!')
        sys.exit(1)
    issue_number = get_linked_issue_number(repo, pr_number)
    if not issue_number:
        print('No linked issue found, proceeding with tag creation.')
        sys.exit(0)
    linked_prs = get_linked_prs_for_issue(repo, issue_number)
    if not linked_prs:
        print('No linked PRs found for the issue, proceeding with tag creation.')
        sys.exit(0)
    for linked_pr in linked_prs:
        if not is_pr_merged(repo, linked_pr):
            print(f'PR #{linked_pr} linked to issue #{issue_number} is not merged. Skipping tag creation.')
            sys.exit(1)
    print('All linked PRs are merged. Proceeding with tag creation.')

if __name__ == '__main__':
    main()

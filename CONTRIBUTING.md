## How to contribute to AO.

Welcome to the AO repository. We are always looking for new developers and hope to make the experience as smooth as possible, 
thank you for considering a contribution to the project. You can follow updates on development both on the Github and in the Discord.

#### **Base guidelines**
* This project is open to anyone who wishes to contribuite for Windows (10/11) and Linux and MacOS. In order to test key features of this program, it is required to have either custom-made assets ready or to obtain them from the official AO discord, found in the #links channel.

* The AO ORG concerns the official client/server and masterserver. If your contribution introduces large changes or is from a fork providing changes up the stream, consider checking out [**AO CHAOS**](https://github.com/AttorneyOnlineChaos). An expiremental branch where most breaking changes are tested.

#### **Issues**

Issues are used for bug-fixes as well as potential feature conversions and requests.

* Open a new GitHub issue following the issue template.

* Include any information relevant to the issue and mention which version of the program (i.e latest master) was used for this test. As well as steps to replicate it.

* Seperate each issue into its own Issue thread and use the "Tags" appropriatley.

* The issue will be looked over by any currently active developer or maintainer, and then an idea of a potential solution is put forward. Discussion over this should be concise and to the point, double posting and flooding is highly discouraged. This process may take some time depending on the availability of developers to tend to the issue.

#### **PRs**

* If you have an idea on how an issue or feature can be handled, open a pull request. This pull request can come from these sources:
  - A branch of the main repo appropriatley titled (i.e Feature-Add-Loading-Bars) or (i.e BugFix-Viewport-Rounding-Error).
  - A fork of the repository with a branch that only contains the relevant changes for the issue. Forks merging with irrelevant or "extra" changes outside of the scope of the issue will be rejected.

* The pull request should present at minimum a build-ready sample for reviewers to look over and test on their own enviroments. "WIP" and larger PRs are allowed, but should likely stay in their own branch or fork. Submitting them is an acknowledgement that they should be reviewed as is.

* Pull requests are first reviewed by active developers or maintainers, and then discussed for further changes (done by the submitter or the developer reviewing) and then accepted by a maintainer. If all checks pass, it is merged into master.

* Some Pull requests may introduce breaking changes to the program, which will result in them being considered but sent to an expiremental branch first before reaching master.

#### **Code Guidelines**

* Code should contribuite towards adding a feature, revamping a feature, solving an existing issue, or preventing a future one. Code cleanliness and concise annotations will help developers work off one another and increase overrall code base health. It doesn't hurt to ask any maintainers if a particular idea is worth it via the Issues tab before making a pull request about it!

* Submitted contributions should at minimum show that the submitter has a grasp of basic coding principles, the codebase they're working with, and the issue at hand. The review process may include questions about how exactly the code functions and checks for edge cases, so it is recommended to be detailed in submission descriptions.

However, not all issues require a complex understanding of all that the codebase entails, and making mistakes and having differing opinions is part of the process. Contributors are encouraged to give their issue of choice a shot even if they don't fully understand it, each step helps. As long as the submitter and reviewer remain civil and amicable. Faliure to do so will recieve punishment.

* Contributed code should attempt to follow the conventions used by the code-base, such as naming schemes, styles and so on. This is to encourage consistency and lessen the burden of figuring out what each snippet does, but may be broken if sufficent reasoning is given. Clang is used to format and build checks will assist in checking any obvious issues.

* Contributed code should NOT include excessive annotations and comments, include vague or non-descriptive naming schemes (i.e Variable A, B, C.. Function getMagic().) and should attempt to address the topic with as simple a solution as possible until complex ones are required.

#### **Third-party Code**
Code that isn't the submitter's. Usually helpful if a problem has existed before or needs an outline for a potential solution.

* Contributions **MUST** disclaim if any snippet or part of this contribution was not made by the submitter. This can include code coming from other forks downstream, helped by a co-author, from online sources (Stackoverflow etc..), and from LLMs/Copilot or Other PRs using third-party code. Attatch the relevant sources in the description of the contribution in any form applicable. Failure to disclose this will result in punishment.

* If you are considering using third-party code, The contributor will have to prove that this snippet is relevant to the topic at hand and that they understand how it achieves that goal.
  
*  It is recommend to clean up the addition as that will help expedite the review process in the case that the submitter doesn't understand fully how it works, such as:
 - Remove or convert excessive annotations and comments that are either overtly descriptive or mention things that are not relevant. (i.e //checks if true (if true) { code })
 - Ensure all additions or removals are in the correct files, and no extra files are created for little reason, or removed.
 - Ensure all added functions, variables, etc are given proper names that follow code-base structure, and do not violate existing standards without good reason.
 - Ensure as many parts of the contribution as possible do not have an over-reliance on templates, seemingly new and random includes into files, excessive function overloading and random guards or checks for unreasonable or unlikely events.
 - Acknowledge the risks involved and that contributions using these methods may be superseded. All credit is to be put appropriatley by both submitters, reviwers and maintainers, even if a PR is rejected.

* Contributions using third-party code will automatically be tagged with "Expiremental" and treated as the WIP basis for the solution until proven otherwise and overseen by an active developer, maintainer with the aid of the submitter. Generally, it can be a good first-step to look at how others approached the problem, and then attempt to apply that here. However, it is also generally bad practice to lean on third-party code that "Just works".

We hope to see your contribution in the future, best of luck!

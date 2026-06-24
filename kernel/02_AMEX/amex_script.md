# AMEX Kernel 4 구조 발표 대본
> EMV Contactless Book C-4 | Kernel 4 Specification v2.10
> 대상: 결제 단말 개발자 및 브랜드 인증 엔지니어

---

## 슬라이드 1 - American Express Contactless Kernel 4

오늘은 American Express 컨택리스 결제에 사용되는 EMV Contactless Book C-4, Kernel 4의 구조와 거래 처리 절차를 살펴보겠습니다.

EMV Contactless Book C-4  |  Kernel 4 Specification v2.10  |  March 2021

상세 기술 교육 자료

---

## 슬라이드 2 - 목차

발표는 사양의 처리 순서에 맞춰 전반부와 후반부로 나눕니다. 전반부에서는 카드 데이터를 읽고 위험을 평가하기 전까지를, 후반부에서는 승인 결정과 온라인 처리, 구현 관점을 다룹니다.

전반부

1. Kernel 4 개요

2. Contactless Modes와 거래 흐름

3. GPO와 Capability 결정

4. Read Application Data

5. Offline Data Authentication

6. Processing Restrictions

후반부

7. Cardholder Verification

8. Terminal Risk Management

9. 1st Terminal/Card Action Analysis

10. Online 및 2nd Action Analysis

11. APDU와 Configuration Data

12. 동작 예시와 구현 체크리스트

---

## 슬라이드 3 - 1. Kernel 4 개요

먼저 Kernel 4가 담당하는 범위와 다른 EMV Contactless 구성요소 사이의 경계를 정리하겠습니다.

---

## 슬라이드 4 - Kernel 4의 역할과 범위

Kernel 4는 RF 프로토콜 자체나 온라인 승인 시스템 전체를 정의하지 않습니다. 선택된 애플리케이션의 데이터를 해석하고, 위험 판단을 수행하며, 카드에 어떤 Cryptogram을 요청할지 결정하는 것이 핵심입니다.

Kernel 4는 American Express Contactless Payment Application과 상호 운용되는 Reader Kernel입니다. Entry Point가 애플리케이션을 선택한 이후 거래 결과를 반환할 때까지의 처리 규칙을 정의합니다.

핵심 역할

• 카드와 C-APDU/R-APDU 교환

• 카드·단말 데이터 검증

• ODA, CVM, Risk Management 수행

• TC, ARQC, AAC 요청 및 결과 처리

사양 경계

• Book A: 공통 아키텍처

• Book B: Entry Point와 Kernel 선택

• Book C-4: Kernel 4 거래 처리

• 온라인 호스트와 UI 구현은 외부 영역

출력

• 승인, 온라인 요청, 거절

• Try Another Interface / End Application

• Data Record와 UI 지시 정보

---

## 슬라이드 5 - Book C-4 v2.10 구성

Book C-4는 카드 접근 순서와 의사결정 순서를 거의 그대로 장으로 나눕니다. 구현에서는 장별 함수를 독립적으로 만들되 TVR, TSI, CVM Results와 같은 공통 데이터가 다음 단계로 이어지도록 관리해야 합니다.

주요 처리 장

• Chapter 2: Contactless Modes와 거래 흐름

• Chapter 3: GPO

• Chapter 4: Initiate Application

• Chapter 5: Read Application Data

• Chapter 6: Offline Data Authentication

• Chapter 7: Processing Restrictions

승인 결정 장

• Chapter 8: Cardholder Verification

• Chapter 9: Terminal Risk Management

• Chapter 10: 1st Terminal Action Analysis

• Chapter 11: 1st Card Action Analysis

• Chapter 12: Online Processing

완료와 부록

• 2nd Terminal/Card Action Analysis와 결과 처리

• Annex A: Transaction Data

• Annex B: Terminal/Entry Point Configuration

• Annex C: Glossary

---

## 슬라이드 6 - 2. Contactless Modes와 거래 흐름

이제 Kernel 4가 지원하는 거래 모드와 시작부터 종료까지의 큰 흐름을 보겠습니다.

---

## 슬라이드 7 - Kernel 4 전체 처리 흐름

거래는 초기화, 카드 기능 파악, 데이터 읽기, 위험 판단, 승인 결정의 다섯 단계로 볼 수 있습니다. 각 단계의 실패는 즉시 거절로 끝나기도 하고 다른 인터페이스 시도나 온라인 요청으로 전환되기도 합니다.

Entry Point가 AID와 Kernel 4를 선택하면 Kernel은 Terminal Data와 카드 응답을 결합해 처리 단계를 순차적으로 수행합니다.

초기화

Transaction Data 수신

Terminal 설정 적용

카드 초기 처리

GPO로 AIP/AFL 획득

지원 모드 결정

위험 판단

데이터 읽기, ODA, 제한 검사

CVM과 Risk Management

승인 결정

Terminal/Card Action Analysis

Online 또는 최종 결과

거래 결과는 카드의 Cryptogram과 Kernel 검증 결과가 모두 일치해야 확정됩니다.

슬라이드의 표는 다음 순서로 읽습니다.
- 1. Initiate: 거래 데이터와 Terminal Capability 준비
- 2. GPO: AIP/AFL과 카드 기능 획득
- 3. Read/Authenticate: 레코드 읽기와 카드 진위 확인
- 4. Verify/Risk: CVM, 제한, 위험 관리
- 5. Action Analysis: TC/ARQC/AAC 결정

---

## 슬라이드 8 - Contactless Modes와 거래 조합

Kernel 4는 단순히 카드가 제시한 모드를 따르지 않습니다. 카드와 Reader의 Capability, Terminal Type, 거래 금액, Zero Amount와 Status Check 설정을 함께 비교해 실제 가능한 조합을 결정합니다.

Kernel 4는 카드 Capability, Terminal 설정, 거래 유형에 따라 지원 가능한 조합을 판정합니다.

EMV Mode

AIP 기반 기능 사용

ODA와 AC 생성 지원

Contactless Mag-Stripe Mode

온라인 전용 처리

동적 Track 데이터 사용

Mobile Transaction

Mobile CVM 결과와

카드 Capability 확인

Offline / Online

Terminal Type과 한도에 따라

TC 또는 ARQC 경로

Delayed Authorisation

설정된 환경에서

후속 온라인 전송

Partial Online

온라인 승인 전후를

분리해 처리

지원 불가 조합은 거래를 계속하지 않고 명시된 Outcome으로 종료합니다.

슬라이드의 표는 다음 순서로 읽습니다.
- EMV Offline: Offline capable + 한도 내: TC 또는 AAC
- EMV Online: Online capable: ARQC 후 최종 AC
- Mag-Stripe: 카드/Reader 모두 지원: Online Request
- Mobile EMV: Mobile 지원 + CVM 결과: CVM 결과 반영
- Delayed Auth: 해당 Terminal 설정: 후속 전송용 결과

---

## 슬라이드 9 - 3. GPO와 Capability 결정

거래의 첫 카드 명령인 GET PROCESSING OPTIONS와 그 응답 처리 규칙을 살펴보겠습니다.

---

## 슬라이드 10 - GPO 처리 - Pre-PDOL부터 응답 검증

PDOL은 카드가 거래 시작 전에 요구하는 Terminal Data 목록입니다. Kernel은 데이터베이스에서 각 값을 찾아 순서대로 연결하고 83 Template으로 감싼 뒤 GPO를 전송합니다. 응답에서는 형식과 필수 데이터, AIP와 AFL의 일관성을 먼저 검증합니다.

Kernel

Terminal Data

PDOL

83 Template

Card

ACT Data

DB 초기화

요청 태그 조회

값 연결

GPO 전송

AIP/AFL 반환

응답 파싱

모드 결정

◀ 데이터

▶ C-APDU

◀ R-APDU

누락 데이터는 규칙에 따라 0 또는 빈 값으로 구성

슬라이드의 표는 다음 순서로 읽습니다.
- 1: ACT/Configuration: Kernel DB 초기화
- 2: PDOL: 요청 Tag 순서 확인
- 3: Terminal Data: 길이에 맞춰 값 구성
- 4: 83 Template: GPO Data Field 생성
- 5: R-APDU: Format 1/2 파싱
- 6: AIP/AFL: Capability와 레코드 결정
- 7: 오류: 명세 Outcome으로 종료
- 8: 정상: 다음 처리 단계로 진행
- 9: 로그: 결정 데이터 보존

---

## 슬라이드 11 - GPO 입력과 응답 검증

GPO는 이후 모든 처리의 기준점을 만듭니다. 특히 AIP 비트는 ODA와 CVM 지원 여부를 결정하고, AFL은 Read Application Data의 범위를 결정하므로 파싱 오류를 느슨하게 허용하면 안 됩니다.

GPO 입력 데이터

• PDOL이 없으면 83 00 사용

• PDOL이 있으면 요구 길이에 맞춰 값 연결

• Modified Terminal Type 또는 Enhanced Reader Capabilities를 조건에 따라 제공

• 거래 금액, 통화, 국가, 유형 등 핵심 데이터 포함

응답 데이터

• AIP: 카드 지원 기능 비트

• AFL: 읽을 SFI/Record 범위

• Format 1 또는 Response Message Template Format 2

• 중복, 길이, 허용되지 않은 태그 검사

실패 처리

• Status Word 오류

• 필수 AIP/AFL 부재

• 형식 또는 길이 오류

• 지원되지 않는 거래 조합

→ End Application 또는 Try Another Interface

---

## 슬라이드 12 - Terminal Type과 Reader Capability

Capability 값은 단순 광고 데이터가 아니라 카드가 AC 유형과 처리 경로를 판단하는 입력입니다. 실제 지원하지 않는 기능을 켜거나 지원 기능을 누락하면 인증 테스트와 실제 거래 모두에서 분기 오류가 발생합니다.

Terminal Type 9F35

• 금융기관/가맹점/카드소지자 환경

• Attended 또는 Unattended

• Online/Offline Capability 표현

Contactless Reader Capabilities 9F6D

• EMV 및 Mag-Stripe 지원

• Mobile 기능 지원

• CVM과 거래 조합 판단에 사용

Modified Terminal Type

• 거래 조건에 따라 원 Terminal Type을 보정

• Zero Amount, Status Check, Delayed Authorisation 반영

Enhanced Reader Capabilities

• 추가 Reader 기능을 카드에 알림

• 지원 비트는 실제 구현 및 설정과 일치해야 함

---

## 슬라이드 13 - 4. Read Application Data

AFL을 기준으로 카드 레코드를 읽고 거래에 필요한 데이터를 수집하는 단계를 설명합니다.

---

## 슬라이드 14 - READ RECORD와 GET DATA 처리

AFL의 순서와 Offline Authentication Record 수를 정확히 보존해야 합니다. 읽은 데이터는 단순 저장하는 것뿐 아니라 ODA용 Static Data 구성과 후속 CVM, Risk Management에 사용되므로 출처와 유효성을 함께 관리해야 합니다.

AFL 순회

• SFI와 시작/종료 Record 번호 확인

• 각 Entry를 순서대로 READ RECORD

• Offline Authentication 대상 Record 표시를 별도 관리

레코드 검증

• Response Message Template 70 확인

• 중복 Tag와 잘못된 길이 검사

• 필요한 데이터는 Kernel DB에 저장

• 알려지지 않은 독점 데이터 처리 규칙 적용

GET DATA

• Application Transaction Counter 등 추가 데이터 조회

• 지원 여부와 응답 Status Word 확인

• 선택적 데이터 실패가 전체 거래 실패인지 구분

---

## 슬라이드 15 - 5. Offline Data Authentication

카드와 Issuer 데이터의 진위를 오프라인에서 검증하는 ODA 처리를 살펴보겠습니다.

---

## 슬라이드 16 - ODA 처리 개요

ODA는 거래가 온라인 호스트에 도달하지 않더라도 위조 카드와 변조 데이터를 탐지하기 위한 공개키 기반 검증입니다.

슬라이드의 표는 다음 순서로 읽습니다.
- ODA 미수행: 지원/키/조건 부족 확인: Not performed 설정
- SDA: Signed Static Application Data 검증: 실패 시 SDA failed
- DDA: ICC Dynamic Number 서명 검증: 실패 시 DDA failed
- CDA: AC와 동적 서명 결합 검증: 실패 시 CDA failed
- CA Key 없음: RID/Index 조회 실패: ODA 불가
- Issuer Cert 오류: 인증서 복원/만료/폐기 검사: 인증 실패
- ICC Cert 오류: ICC Public Key 복원 실패: 인증 실패
- 정상: Static/Dynamic Data 일치: 후속 처리 계속
- 결과 사용: TVR에 반영: Action Analysis 입력

---

## 슬라이드 17 - 인증서 체인과 인증 방식

CDA는 AC 생성과 동적 인증을 결합하므로 승인 결과 자체를 보호합니다. 구현에서는 인증서 복원, Hash 입력 조립, 길이와 Padding 검사를 독립 모듈로 분리해 테스트하는 것이 중요합니다.

공개키 체인

• Scheme CA Public Key로 Issuer Certificate 복원

• Issuer Public Key로 ICC Certificate 또는 SDA 데이터 검증

• PAN, 만료일, Algorithm Indicator, Hash 일치 확인

• Revocation List로 폐기 인증서 확인

SDA / DDA

• SDA: 정적 Application Data의 서명 검증

• DDA: Internal Authenticate 기반 동적 서명 검증

• 예측 불가능한 데이터로 복제 카드 방어

CDA

• GENERATE AC 응답의 SDAD 검증

• AC와 Terminal Dynamic Data를 하나의 서명으로 결합

• 거래 결과와 카드 인증을 동시에 보호

---

## 슬라이드 18 - 6. Processing Restrictions

카드 사용 조건과 Reader의 Dynamic Limit을 적용해 거래 허용 여부를 판단합니다.

---

## 슬라이드 19 - Dynamic Reader Limits

Dynamic Reader Limits는 하나의 금액 기준이 아닙니다. 거래 허용, CVM 요구, 온라인 전환이라는 세 가지 결정을 서로 다른 한도로 제어하므로 비교 결과를 별도 상태로 저장해야 합니다.

Limit 선택

• AID, 거래 유형, Reader 설정에 따라 Limit Set 선택

• Contactless Transaction Limit

• CVM Required Limit

• Floor Limit을 개별 적용

동적 업데이트

• 카드 또는 거래 조건에 따라 Reader Limit Indicator 갱신

• 금액 비교 결과를 후속 CVM과 Risk Management에서 사용

• Limit 미지원 Reader는 정적 설정 사용

거래 제한

• Transaction Limit 초과: Contactless Application Not Allowed

• CVM Limit 초과: CVM 필요 표시

• Floor Limit 초과: Online 경로 우선

---

## 슬라이드 20 - AUC, 버전, 유효기간 검사

Processing Restrictions는 한 단계의 승인/거절 결론이 아니라 위험 신호를 TVR에 기록하는 과정입니다. 단, 명세에서 즉시 종료하도록 정의한 불가능 조합은 Action Analysis까지 진행하지 않습니다.

Application Version

• 카드와 Terminal의 Application Version Number 비교

• 불일치 시 TVR 설정

• 바로 종료하지 않고 Action Analysis에 반영 가능

Application Usage Control

• 국내/국제 거래 허용

• 현금, 상품, 서비스, Cashback 허용

• ATM/비ATM 환경 조건 검사

날짜 검사

• Application Effective Date 이전 사용 확인

• Application Expiration Date 경과 확인

• Terminal Date 형식과 BCD 유효성 검증

결과

Processing Restrictions 결과는 TVR에 누적되고 이후 IAC/TAC 비교에 사용됩니다.

---

## 슬라이드 21 - 7. Cardholder Verification

카드소지자 인증 방식 선택과 Mobile CVM 결과 검증을 설명합니다.

---

## 슬라이드 22 - CVM Process Control

CVM 선택은 카드 CVM List만 따라가는 접촉식 처리와 다를 수 있습니다. Kernel 4에서는 Reader Limit과 Mobile 결과, 카드 Capability를 함께 사용해 최종 CVM을 정합니다.

CVM 필요성

• 거래 금액과 Reader CVM Required Limit 비교

• 카드의 CVM Capability와 Terminal Capability 확인

• 거래 유형과 환경에 맞는 방법 선택

지원 방식

• No CVM

• Signature

• Online PIN

• Consumer Device CVM / Contactless Mobile CVM

• CVM 불가 결과

결과 기록

• CVM Results 9F34 갱신

• Mobile CVM Results 9F71 검증

• 실패/미지원 상태를 TVR에 반영

---

## 슬라이드 23 - Mobile CVM과 Reader Limit 검증

Mobile 화면에서 인증되었다는 사실만으로 CVM 성공을 선언하면 안 됩니다. 9F71 값, 거래 Context와 Reader 설정을 모두 확인하고, 명세가 요구하는 결과 조합만 유효하게 받아들여야 합니다.

Mobile CVM

• 기기에서 PIN 또는 생체 인증 수행

• 9F71의 Method, Result, Context 해석

• 거래 금액과 Mobile CVM 가능 조건 비교

Reader CVM Required Limit

• 금액이 한도 이하이면 No CVM 가능

• 한도 초과 시 카드/기기/Terminal 공통 지원 CVM 필요

• 지원 방식이 없으면 CVM Unable To Complete

Final Outcome

• CVM 성공: 선택 방식 기록

• 실패: TVR와 Outcome Parameter 반영

• 카드 재제시나 다른 인터페이스가 필요한 경우 명시된 UI 요청

슬라이드의 표는 다음 순서로 읽습니다.
- 한도 이하: No CVM 지원: 지원: No CVM
- 한도 초과: Mobile CVM 성공: 지원: Mobile CVM
- 한도 초과: Online PIN 지원: 지원: Online PIN
- 한도 초과: 공통 방식 없음: -: CVM 불가

---

## 슬라이드 24 - 8. Terminal Risk Management

Terminal이 수행하는 금액 및 빈도 기반 위험 평가를 정리합니다.

---

## 슬라이드 25 - Terminal Risk Management 구성

Terminal Risk Management의 각 검사는 독립적으로 실패할 수 있습니다. 여러 결과를 하나의 Boolean으로 합치지 말고 TVR의 정해진 비트에 누적해야 IAC/TAC 비교가 정확해집니다.

Floor Limit Checking

거래 금액이 Floor Limit을 초과하면 Online 필요 신호를 TVR에 설정합니다.

Random Transaction Selection

한도 이하 거래 일부를 무작위로 온라인 전환해 오프라인 공격을 제한합니다.

Velocity Checking

ATC와 Last Online ATC를 이용해 연속 오프라인 거래 횟수를 평가합니다.

Exception File

PAN 또는 카드 식별자가 예외 목록에 있으면 위험 신호를 설정합니다.

결과 누적

각 검사의 결과는 TVR에 누적되고 1st Terminal Action Analysis에서 사용됩니다.

---

## 슬라이드 26 - 9. 1st Terminal Action Analysis

첫 번째 GENERATE AC 전에 Terminal이 요청할 Cryptogram 유형을 결정합니다.

---

## 슬라이드 27 - TVR와 IAC/TAC 비교

Action Analysis는 TVR의 의미를 Issuer와 Terminal 정책에 연결하는 단계입니다. Denial 조건을 먼저 평가한 뒤 온라인 가능 여부와 Default Code를 적용해야 합니다.

Denial 판단

• TVR AND (IAC-Denial OR TAC-Denial)

• 일치 비트가 있으면 AAC 요청

Online 판단

• Denial 조건이 없을 때 Online Code 비교

• 일치하면 ARQC 요청

• Online 불가 시 Default Code 비교

Offline 판단

• Denial/Online 조건이 없고 Offline 가능

• TC 요청

• Terminal Type과 거래 조합 조건 확인

우선순위

Denial → Online → Default/Offline 순서로 평가하며 카드가 요청한 조건과 Zero Amount/Status Check 규칙을 추가 반영합니다.

---

## 슬라이드 28 - 1st GENERATE AC 요청 결정

Terminal이 요청한 AC는 최종 결과가 아닙니다. 카드는 같은 유형 또는 더 보수적인 유형을 반환할 수 있으므로 응답을 다시 검증하고 1st Card Action Analysis를 수행해야 합니다.

요청 AAC

• 명확한 거절 조건

• 지원되지 않는 거래 또는 치명적 검증 실패

• P1의 Cryptogram Type을 AAC로 설정

요청 ARQC

• 온라인이 필요하거나 정책상 온라인 선택

• CDOL1 Related Data 구성

• CDA 요청 비트는 조건 충족 시 함께 설정

요청 TC

• 오프라인 승인 가능

• 모든 제한과 위험 판단 통과

• 카드가 TC 대신 ARQC/AAC를 반환할 수 있음

명령 구성

CDOL1의 Tag 순서와 길이에 맞춰 데이터 필드를 만들고, 누락 데이터는 해당 규칙대로 채웁니다.

---

## 슬라이드 29 - 10. 1st Card Action Analysis

카드가 반환한 첫 번째 Cryptogram을 해석하고 다음 처리 경로를 결정합니다.

---

## 슬라이드 30 - GENERATE AC 응답 형식과 검증

응답 파싱은 단순 TLV 저장이 아닙니다. 요청한 AC와 카드 반환 AC의 허용 관계, CDA 결과, 필수 데이터 일관성을 모두 검사한 뒤 Outcome을 정해야 합니다.

응답 형식

• Response Message Template Format 1 또는 2

• CID, ATC, AC, IAD 확인

• CDA이면 SDAD 포함 및 서명 검증

Cryptogram Type

• AAC: 카드 거절

• TC: 카드 오프라인 승인

• ARQC: 온라인 승인 요청

• 요청보다 허용되지 않은 상향 결과는 오류

데이터 검증

• 필수 Tag, 길이, 중복 확인

• CID의 Advice/Reason Code 해석

• 결과 Data Record와 TVR/TSI 갱신

---

## 슬라이드 31 - 11. Online 및 2nd Action Analysis

ARQC 이후 Host 응답을 카드에 반영하고 최종 승인 또는 거절을 확정합니다.

---

## 슬라이드 32 - Online Processing

Online Processing은 Kernel 외부에서 수행되지만 Kernel은 Host에 필요한 정확한 Data Record를 제공하고 응답을 검증해야 합니다. 통신 실패도 별도의 결과로 처리해 Default 정책에 연결합니다.

Online Request Data

• ARQC, ATC, IAD, AIP, TVR, 거래 금액 등 전송

• Mag-Stripe Mode에서는 동적 Track 데이터와 ATC/AC 검사

Host Response

• Authorisation Response Code

• Issuer Authentication Data

• Issuer Script Template 1/2

• Online 처리 성공/실패 상태

Kernel 재개

Terminal이 온라인 결과를 제공하면 Kernel DB를 갱신하고 Issuer Authentication, Script, 2nd Action Analysis 순서로 진행합니다.

---

## 슬라이드 33 - 12. 2nd Terminal/Card Action Analysis

온라인 응답을 반영해 두 번째 GENERATE AC 요청과 카드의 최종 결과를 처리합니다.

---

## 슬라이드 34 - 2nd Terminal Action Analysis

두 번째 Terminal Action Analysis는 Host 결과를 그대로 복사하지 않습니다. Issuer Authentication, Script 결과와 Online 실패 정책을 합쳐 카드에 요청할 최종 AC를 결정합니다.

Host 승인

• ARC가 승인이고 치명적 오류가 없으면 TC 요청

• Issuer Authentication/Script 결과를 반영

• CDOL2 Related Data 구성

Host 거절

• AAC 요청

• 거절 ARC와 Issuer 결과 보존

Online 실패

• Terminal Type과 Default 정책 적용

• IAC/TAC-Default 비교

• 오프라인 승인 가능 여부 결정

요청 제한

첫 번째 결과와 카드 Capability에 따라 허용되는 2nd AC 유형을 지키고, CDA 조건을 다시 확인합니다.

---

## 슬라이드 35 - 2nd Card Action Analysis와 최종 Outcome

최종 Outcome은 카드의 두 번째 AC가 기준입니다. Host가 승인했더라도 카드가 AAC를 반환하면 거래는 거절이며, 모든 결과 데이터가 이 결론과 일치해야 합니다.

최종 TC

• 승인 Outcome

• Receipt와 UI 요청 설정

• 최종 Data Record 생성

최종 AAC

• 거절 Outcome

• Reason/Advice와 Error Indication 반영

응답 오류

• Format, CID, 필수 데이터, CDA 검증 실패

• End Application 또는 명세된 오류 Outcome

완료

TSI/TVR, CVM Results, AC, Issuer Script Results 등 최종 데이터를 Terminal에 반환합니다.

---

## 슬라이드 36 - 13. APDU와 Configuration Data

구현에서 직접 다루는 카드 명령, 주요 Tag와 설정 데이터를 정리합니다.

---

## 슬라이드 37 - Kernel 4 주요 C-APDU

APDU 계층은 명령 조립, 전송, Status Word 평가를 분리해 구현하는 것이 좋습니다.

Kernel은 거래 단계와 카드 Capability에 따라 표준 EMV 명령을 순서대로 전송합니다.

슬라이드의 표는 다음 순서로 읽습니다.
- GET PROCESSING OPTIONS: A8: PDOL Related Data: AIP, AFL
- READ RECORD: B2: SFI, Record: Application Data
- GET DATA: CA: Tag: ATC 등
- INTERNAL AUTHENTICATE: 88: DDOL Data: Dynamic Signature
- GENERATE AC: AE: CDOL1/2 Data: CID, ATC, AC, IAD
- EXTERNAL AUTHENTICATE: 82: Issuer Auth Data: Status Word
- GET CHALLENGE: 84: -: Unpredictable Data
- Issuer Script Command: var.: Script APDU: Command Result

---

## 슬라이드 38 - 핵심 데이터 오브젝트와 설정

Tag 저장소는 값뿐 아니라 존재 여부, 출처, 길이 검증 상태를 관리해야 합니다. Configuration은 AID와 거래 유형별로 분리하고 인증 환경과 운영 환경의 값 차이를 추적해야 합니다.

거래 데이터

• 9F02 Amount Authorised

• 9C Transaction Type

• 5F2A Transaction Currency

• 9A Transaction Date

• 9F37 Unpredictable Number

카드/결과 데이터

• 82 AIP, 94 AFL

• 9F27 CID, 9F36 ATC, 9F26 AC

• 9F10 IAD

• 95 TVR, 9B TSI, 9F34 CVM Results

Terminal Configuration

• Terminal Type 9F35

• Reader Capabilities 9F6D

• Transaction/CVM/Floor Limits

• TAC Denial/Online/Default

• CA Public Keys와 Revocation List

---

## 슬라이드 39 - Configuration Data와 구현 보호 장치

Configuration 오류는 카드 응답 오류처럼 보이는 분기를 만들 수 있습니다. 설정 버전과 인증 Test Profile을 함께 관리하고 거래 로그에 적용된 설정 식별자를 남기는 것이 좋습니다.

필수 설정

• Terminal Type과 Reader Capability

• Transaction, CVM, Floor Limits

• TAC Denial/Online/Default

• 지원 Version과 Country/Currency

보안 자산

• RID/Index별 CA Public Key

• 만료일과 Hash Algorithm

• Certificate Revocation List

• 운영 중 안전한 갱신 절차

구현 보호

• APDU Timeout과 재시도 정책

• Tag 중복/길이/형식 오류 차단

• 거래별 DB 초기화

• 민감 데이터 로그 최소화

---

## 슬라이드 40 - 14. 동작 예시와 구현 체크리스트

마지막으로 대표적인 거래 흐름을 단계별로 연결하고 구현 시 확인할 항목을 정리합니다.

---

## 슬라이드 41 - 동작 예시 1 - 소액 EMV 오프라인 승인

소액 거래라고 항상 오프라인 승인되는 것은 아닙니다. 카드와 Terminal의 기능, ODA와 CVM 결과, IAC/TAC가 모두 허용해야 TC가 최종 반환됩니다.

Processing Restrictions와 Risk Management 결과가 양호하고 Terminal/Card가 Offline을 지원하는 경우

슬라이드의 표는 다음 순서로 읽습니다.
- 1: GPO: AIP/AFL 획득
- 2: READ RECORD: 카드 데이터 수집
- 3: ODA: 카드 인증 성공
- 4: Restrictions: 금액/날짜/AUC 통과
- 5: CVM: No CVM 또는 지원 방식 성공
- 6: Risk Management: Floor/Random/Velocity 통과
- 7: 1st TAA: TC 요청 결정
- 8: GENERATE AC: 카드가 TC 반환
- 9: CDA: 해당 시 서명 검증
- 10: Outcome: Approved
- 11: Data Record: 정산 데이터 구성
- 12: UI: 승인 메시지 표시

---

## 슬라이드 42 - 동작 예시 2 - 고액 EMV 온라인 승인

고액 거래에서는 CVM과 온라인 처리가 연결됩니다. 첫 번째 ARQC와 Host 응답, 두 번째 TC가 각각 독립적으로 유효해야 최종 승인됩니다.

CVM Required Limit 또는 Floor Limit을 초과해 ARQC를 요청하고 Host 승인 후 TC로 완료하는 흐름

슬라이드의 표는 다음 순서로 읽습니다.
- 1: GPO/READ: Capability와 데이터 수집
- 2: Restrictions: Transaction Limit 이내
- 3: CVM: Online PIN 또는 Mobile CVM
- 4: Risk Management: Online 필요 설정
- 5: 1st TAA: ARQC 요청
- 6: 1st CAA: ARQC 검증
- 7: Online Request: Host 데이터 전송
- 8: Host: 승인 ARC 수신
- 9: Issuer Auth: 지원 시 수행
- 10: Script 1: 사전 Script 처리
- 11: 2nd TAA: TC 요청
- 12: 2nd CAA: TC 검증
- 13: Script 2: 사후 Script 처리
- 14: Outcome: Approved

---

## 슬라이드 43 - 동작 예시 3 - 거절과 Try Another Interface

거절과 Try Another Interface는 사용자 경험과 후속 처리 의미가 다릅니다. 결제 자체가 거절된 것인지, Contactless 인터페이스만 사용할 수 없는 것인지 Outcome과 UI 메시지를 정확히 구분해야 합니다.

카드가 AAC를 반환하거나 Contactless 처리 조건이 충족되지 않는 경우의 분기

거절 경로

① 치명적 검증 실패 또는 Denial Code 일치

② Terminal이 AAC 요청

③ 카드가 AAC 반환

④ Not Authorised UI

⑤ Declined Outcome

다른 인터페이스 경로

① 카드/Reader 조합 또는 응답이 Contactless 처리 불가

② Error Indication과 Start 파라미터 설정

③ Try Another Interface UI

④ Contact 또는 다른 결제 수단으로 전환

---

## 슬라이드 44 - 동작 예시 4 - Mobile CVM과 Dynamic Limits

Mobile CVM 성공 표시만 보고 한도를 무시해서는 안 됩니다. Transaction Limit, CVM Required Limit, Floor Limit은 각각 거래 허용, 인증 요구, 온라인 전환을 제어합니다.

Mobile 거래에서는 기기 인증 결과와 Reader Limit을 함께 검증합니다.

슬라이드의 표는 다음 순서로 읽습니다.
- 1: Mobile Capability 확인: Mobile 경로 활성화
- 2: Transaction Amount: Limit Set 선택
- 3: Transaction Limit 초과: Contactless 불가
- 4: CVM Limit 이하: No CVM 가능
- 5: CVM Limit 초과: 9F71 검증
- 6: Mobile CVM 성공: CVM 성공 기록
- 7: Mobile CVM 실패: 대체 CVM 탐색
- 8: 공통 CVM 없음: CVM 불가
- 9: Floor Limit 초과: Online 필요
- 10: ARQC/Host 승인: 2nd AC 진행
- 11: 최종 AC: Approved/Declined

---

## 슬라이드 45 - 동작 예시 5 - Zero Amount와 Status Check

Zero Amount와 Status Check는 금액만 0인 일반 구매가 아닙니다. 지원 플래그와 Terminal Type 보정, 카드 Capability를 별도로 확인해야 인증 Test Case의 기대 Outcome과 일치합니다.

Zero Amount 거래와 Status Check 요청은 일반 구매와 다른 지원 조건을 적용합니다.

Zero Amount

• Reader 설정에서 허용 여부 확인

• Modified Terminal Type 적용 조건 검토

• 카드 Capability와 거래 조합 검사

• 허용되지 않으면 즉시 Contactless 종료

Status Check

• 거래 금액과 Transaction Type 조합 확인

• 온라인 전용 또는 별도 Outcome 규칙 적용

• 일반 구매의 Floor/CVM 판단을 그대로 재사용하지 않음

구현 포인트

거래 시작 단계에서 일반 구매와 분리하고 GPO 입력, Terminal Type, Action Analysis 정책이 일관되게 적용되도록 합니다.

---

## 슬라이드 46 - Kernel 4 구현 체크리스트

구현 품질은 정상 승인 경로보다 실패 분기에서 드러납니다. 데이터 누락, 잘못된 길이, Online 실패, 카드의 예상 밖 AC 반환을 각각 독립 Test Case로 만들어야 합니다.

데이터와 상태

• Tag 존재/길이/출처를 분리 관리

• TVR/TSI 비트는 단계별 누적

• 첫 번째와 두 번째 AC 상태 분리

• AID/거래 유형별 Configuration 적용

명령과 보안

• DOL 순서와 길이 정확성

• 모든 R-APDU 형식과 SW 검사

• CA/Issuer/ICC 인증서 검증

• CDA Hash 입력과 AC 일관성 확인

결과와 인증

• Outcome, UI, Data Record의 결론 일치

• CVM/Limit/Terminal Type 조합 시험

• Offline/Online/실패 분기별 로그

• 브랜드 인증 Test Case와 추적 가능성 유지

---

## 슬라이드 47 - Kernel 4 구조 종합 요약

전체 흐름을 하나의 상태 기계로만 보면 복잡하지만, 데이터 획득, 검증, 위험 평가, AC 결정의 네 책임으로 나누면 구현과 시험이 명확해집니다.

Book C-4 Kernel 4는 AMEX Contactless 거래의 카드 통신, 위험 판단, 승인 결정을 연결하는 핵심 처리 사양입니다.

핵심 처리

• GPO로 Capability와 AFL 결정

• Read/ODA/Restrictions로 카드와 거래 검증

• CVM과 Risk Management로 위험 평가

• IAC/TAC 기반 AC 요청

최종 결과

• 1st AC에서 TC/ARQC/AAC 처리

• ARQC는 Online과 2nd AC로 완료

• Outcome, UI, Data Record를 일관되게 반환

• Dynamic Limits와 Mobile CVM을 정확히 결합

---

## 슬라이드 49 - AMEX Kernel 4 소스코드 핵심 분석

오늘은 기존 Kernel 4 사양 설명에 이어 실제 TNP-10 소스에서 AMEX 거래가 어떻게 구현되어 있는지 보겠습니다. 전체 코드를 나열하지 않고 거래 진입, 설정, 카드 선택, 거래 처리, 승인 판단에 직접 연결되는 코드만 소개하겠습니다.

---

## 슬라이드 50 - 전체 소프트웨어 구조

먼저 구조입니다. Host가 보낸 명령은 interface.c에서 해석되고 EMV2 Kernel API로 전달됩니다. Kernel은 거래 규칙을 수행하지만 실제 카드 송수신은 SendRcvICCAPDU를 통해 Reader Firmware의 RF 처리로 내려갑니다. 이 경계를 기준으로 위쪽은 결제 로직, 아래쪽은 PN5180과 카드 통신이라고 이해하면 됩니다.

---

## 슬라이드 51 - Host 명령 진입점 - interface.c

외부에서 들어오는 AMEX 거래의 시작점은 interface.c입니다. A7부터 A9까지는 Terminal, AID, Revocation 설정을 Kernel에 전달합니다. A2 거래 명령에서는 RF 필드를 켠 뒤 EmvTranProc를 호출하고, 결과를 Host에 전송한 후 RF를 끕니다. 즉 Host 프로토콜과 Kernel 호출의 결합 지점입니다.

---

## 슬라이드 52 - Kernel API와 거래 Dispatcher

Kernel 외부 인터페이스는 매우 작습니다. 초기화와 설정, 거래 Dispatcher, 그리고 PSE·AID·거래·종료 함수로 구성됩니다. EmvTranProc는 한 번의 Host 명령으로 선택과 거래 처리를 순차 호출합니다. 각 단계가 0, 즉 오류 없음일 때만 다음 단계로 넘어가는 구조입니다.

---

## 슬라이드 53 - Terminal과 AMEX 설정 파싱

SetTerminalConfig는 Host가 전달한 TLV형 설정 버퍼를 내부 구조체로 복사합니다. 거래 금액과 통화뿐 아니라 Transaction, CVM, Floor Limit을 별도로 저장합니다. AMEX Mobile CVM과 거래 조합 판단에 쓰이는 9F6E와 9F6D도 여기서 설정됩니다. 이 값이 잘못되면 뒤의 GPO와 CVM 분기가 모두 달라집니다.

---

## 슬라이드 54 - PSE·AID 선택과 GPO

카드 선택 단계에서는 PSE Directory에서 AID와 우선순위를 읽고 Terminal 설정과 비교합니다. 소스 주석에는 AMEX 기본 AID가 아닌 경우를 처리하는 인증 항목이 명시돼 있습니다. 선택 후에는 PDOL을 구성해 GPO를 보내고 AIP, AFL, Enhanced Capability와 Mobile CVM 결과인 9F71을 확보합니다.

---

## 슬라이드 55 - 핵심 거래 처리 - EmvFuncProcTran()

실제 거래의 중심은 EmvFuncProcTran입니다. 카드 레코드와 CVM 데이터를 확인한 뒤 Processing Restrictions와 Mobile CVM 결과를 반영합니다. TerminalActAnalysis의 결과에 따라 GENERATE AC의 P1을 선택합니다. 80 계열은 ARQC, 40 계열은 TC, 00 계열은 AAC이며 CDA 요청 여부에 따라 상위 비트가 추가됩니다.

---

## 슬라이드 56 - PDOL/CDOL과 APDU 생성

PDOL과 CDOL은 같은 DoldataSetting 함수가 타입 값으로 구분해 만듭니다. 카드가 요구한 Tag 순서와 길이에 맞춰 값을 이어 붙이는 방식입니다. SendGenerateAC는 이 결과를 80 AE 명령으로 조립하고 SendRcvICCAPDU를 호출합니다. 이 두 함수가 EMV 데이터 모델을 실제 카드 명령으로 변환하는 핵심입니다.

---

## 슬라이드 57 - CVM과 Mobile CVM 핵심 분기

AMEX 인증 수정이 가장 많이 모인 부분이 CVM입니다. CheckCVMlist는 카드 CVM List뿐 아니라 금액, C00D Limit, Reader Capability 9F6E, Mobile 결과 9F71을 함께 봅니다. 결과는 9F34와 TVR에 기록되고, 조건에 따라 다음 CVM을 시도하거나 거래 재시도로 전환합니다.

---

## 슬라이드 58 - Risk·Action Analysis와 AMEX CAPK

승인 판단은 TVR과 IAC/TAC의 비트 비교로 이뤄집니다. Denial 조건이면 AAC, Online 조건이면 ARQC가 선택됩니다. 카드 인증에는 EMV_CApbkey.c의 AMEX RID A000000025 공개키가 사용됩니다. 최종 TA Pass 버전은 이전 버전 대비 AID Priority, Mobile CVM, CDA 오류 처리와 DOL Padding 관련 수정이 집중돼 있습니다.

---

## 슬라이드 59 - 핵심 코드 요약 & Q&A

정리하면 Host 명령은 interface.c에서 Kernel로 들어오고, 선택과 거래 처리 후 APDU Bridge를 통해 카드로 전달됩니다. AMEX 구현을 볼 때는 Contactless Limit, 9F6E와 9F71의 Mobile CVM 분기, IAC/TAC Action Analysis, AMEX CAPK를 우선 보면 됩니다. 이상으로 핵심 코드 소개를 마치겠습니다.

## 슬라이드 60 - Q & A

이상으로 AMEX Contactless Kernel 4의 구조와 거래 처리 흐름을 마치겠습니다. 질문을 받겠습니다.

EMV Contactless Specifications for Payment Systems

Book C-4 - Kernel 4 Specification v2.10  |  March 2021

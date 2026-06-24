# PCI PTS 인증의 이해 발표 대본

## 슬라이드 1. PCI PTS 인증의 이해
PCI PTS POI는 결제 장치가 PIN과 암호키, 그리고 신청한 민감 기능을 안전하게 처리하는지 평가하는 체계이다. 제품 내부 설계뿐 아니라 제조, 키 주입, 배송과 변경 관리까지 함께 본다.

## 슬라이드 2. PCI PTS POI는 무엇을 검증하는가
POI는 사용자가 카드나 PIN을 제시하는 결제 접점이다. PCI PTS는 이 장치가 물리 침입, 소프트웨어 변조, 키와 PIN 노출, 외부 인터페이스 공격에 견디는지를 검증한다. 운영 환경 전체를 평가하는 PCI DSS와는 중심 대상이 다르다.

## 슬라이드 3. 누가 무엇을 증명하는가
제조사는 제품과 설계 문서, 평가 샘플과 운영 절차를 제공한다. PCI가 인정한 시험소는 요구사항과 DTR에 따라 문서 검토와 공격 시험을 수행한다. PCI SSC는 결과를 검토하고 승인된 모델과 버전, 기능을 Approved PTS Device 정보로 공개한다.

## 슬라이드 4. 장치 유형과 적용 기능을 먼저 정한다
평가를 시작할 때는 최종 제품의 유형과 신청 기능을 먼저 확정한다. PIN 입력 장치인지, 무인 단말인지, 통합 POS인지에 따라 위험이 달라진다. SRED와 Open Protocol 같은 선택 기능을 신청하면 해당 추가 요구사항이 적용된다.

## 슬라이드 5. 무엇을 보호해야 하는가
가장 중요한 기본 자산은 평문 PIN과 암호키, 그리고 보안 설정과 펌웨어이다. PAN과 Track Data는 특히 SRED 기능을 신청한 장치에서 읽는 순간의 암호화와 복호화 통제까지 평가한다. 따라서 모든 PTS 장치가 동일한 PAN 보호 범위를 갖는다고 단정하면 안 된다.

## 슬라이드 6. Security Requirements와 DTR의 관계
Security Requirements는 장치가 달성해야 할 보안 목표를 정의한다. Derived Test Requirements, 즉 DTR은 그 목표를 어떤 증빙과 시험으로 확인할지를 구체화한다. 개발자는 요구사항과 DTR, 관련 Technical FAQ를 함께 읽어야 실제 평가 기대치를 이해할 수 있다.

## 슬라이드 7. 보안 영역은 하나의 체계로 연결된다
Physical Security는 침입과 관찰, Side-Channel을 다룬다. Logical Security는 Software, Key, PIN과 SRED를 다룬다. Interface와 Life-Cycle Security까지 연결되어야 평가된 설계가 제조와 배포 현장에서도 유지된다.

## 슬라이드 8. Tamper 보호는 세 가지 방식으로 구성된다
Tamper Evidence는 공격 흔적을 남기고, Tamper Resistance는 접근 비용을 높인다. Tamper Detection and Response는 Sensor나 Mesh가 침입을 감지하면 키 삭제나 기능 차단을 실행한다. 제품은 위험에 맞게 이 세 방식을 조합한다.

## 슬라이드 9. Attack Potential은 공격 난이도를 점수화한다
Attack Potential은 공격에 필요한 시간, 전문 지식, 장비, 샘플 수와 접근 기회를 평가한다. 시험소는 실제 공격 시나리오의 준비와 실행 비용을 계산한다. 요구된 최소 수준보다 낮은 비용으로 자산을 침해할 수 있다면 보호가 부족하다고 판단한다.

## 슬라이드 10. Side-Channel과 환경 조작도 물리 공격이다
공격자는 전력, 전자기파, 타이밍과 오류 반응에서 키 연산의 단서를 얻을 수 있다. 전압, Clock, 온도나 Laser를 조작해 보안 검사를 건너뛰려 할 수도 있다. 장치는 환경과 센서 상태를 감시하고 허용 범위를 벗어나면 안전 상태로 전환해야 한다.

## 슬라이드 11. Software Management가 신뢰 실행을 만든다
실행되는 Software와 Firmware는 승인된 출처와 무결성을 확인해야 한다. Boot와 실행 중 검증, 버전 식별, Rollback 통제와 변경 이력이 연결되어야 한다. 목표는 승인되지 않은 코드가 설치되거나 실행되는 모든 경로를 차단하는 것이다.

## 슬라이드 12. Secure Boot와 Update는 하나의 신뢰 사슬이다
Secure Boot는 변경하기 어려운 Root of Trust에서 시작해 Bootloader와 Application을 순서대로 검증한다. Update도 서명, 대상 장치, 버전과 무결성을 확인해야 한다. Debug와 복구 경로도 이 신뢰 사슬을 우회해서는 안 된다.

## 슬라이드 13. Key Management는 생성부터 파기까지 이어진다
암호키는 생성, 주입, 저장, 사용, 교체와 파기 전 과정에서 보호한다. 서로 다른 목적의 Key를 분리하고 Dual Control과 Split Knowledge를 적용한다. 승인된 Algorithm, Key Length와 보호 형식도 함께 관리한다.

## 슬라이드 14. PIN Management는 입력 순간을 보호한다
평문 PIN은 필요한 최소 영역과 시간에만 존재해야 한다. 입력 즉시 승인된 PIN Block으로 암호화하고 Buffer와 임시 복사본을 제거한다. Cancel, Timeout, Non-PIN Prompt와 오류 경로가 PIN 보호를 우회하지 않는지도 확인한다.

## 슬라이드 15. SRED는 계정 데이터를 읽는 순간부터 암호화한다
SRED는 카드 Interface에서 읽은 Account Data를 정의된 보호 지점에서 즉시 암호화한다. 평문 PAN과 Track Data의 저장, 표시, 로그와 비인가 복호화를 제한한다. SRED는 선택 모듈이므로 제품의 승인 범위에서 실제 신청 여부를 확인해야 한다.

## 슬라이드 16. 취약점은 출시 후에도 관리한다
보안 평가는 출시 시점의 상태로 끝나지 않는다. 제조사는 공개 취약점과 사용 Library, 공격 동향을 추적하고 영향 분석과 Patch 배포 절차를 운영해야 한다. Secure Development, 권한, Incident와 변경 기록이 실제로 유지되는지도 중요하다.

## 슬라이드 17. Open Protocol은 외부 연결을 신뢰 경계로 본다
외부 Network와 장치가 민감 명령을 보내기 전에 통신 상대를 인증해야 한다. 통신은 기밀성과 무결성을 보호하고 Replay와 Downgrade를 방지한다. USB, Ethernet, Wi-Fi와 원격 관리·Debug 기능은 필요한 서비스와 권한만 열어 둔다.

## 슬라이드 18. 제조 단계에서 장치 신뢰가 시작된다
생산 현장은 승인된 부품과 Firmware, Tool을 사용하고 작업과 샘플의 추적성을 유지해야 한다. 장치 Identity와 초기 Key 주입은 통제된 환경에서 수행한다. 생산 수량과 불량·폐기 제품을 대조해 유실이나 대체를 탐지한다.

## 슬라이드 19. 제조사에서 KIF와 고객까지 신뢰를 이어 간다
출하 장치는 봉인, Serial Number와 운송 기록으로 이동 중 개봉과 교체를 탐지한다. KIF는 대상 장치와 작업 권한을 확인하고 Key가 평문으로 노출되지 않게 주입한다. 설치, 수리와 반품 과정에서도 장치 식별과 민감 데이터 제거가 필요하다.

## 슬라이드 20. 공격을 감지하면 안전한 상태로 전환한다
Tamper나 중대한 오류를 감지하면 민감 Key와 데이터가 제거되어야 한다. 장치는 보안이 약해진 상태로 결제를 계속하지 않고 민감 기능을 비활성화한다. 복구는 인증된 절차와 권한, 정상 Firmware와 Key 재주입을 통해서만 수행한다.

## 슬라이드 21. Approved PTS Device 정보는 범위를 확인하는 문서다
승인 목록에서는 모델, Hardware와 Firmware Version, 승인 상태를 확인한다. PIN, SRED, Open Protocol 등 실제 승인된 기능과 사용 조건도 함께 본다. 외형과 제품명이 같아도 Version이나 기능이 다르면 같은 승인으로 간주할 수 없다.

## 슬라이드 22. 평가는 주장보다 추적 가능한 증빙을 본다
평가 자료는 Architecture와 Data Flow, 회로와 Tamper 구조, Key Flow를 설명해야 한다. 구현 자료는 Source와 Binary, Build와 Signing, Version과 시험 결과를 연결한다. Requirement에서 Design, Implementation, Test Result까지 추적되어야 한다.

## 슬라이드 23. 승인 후 변경도 보안 영향으로 판단한다
Hardware, Firmware, Library, 공급업체와 제조 위치의 변경을 모두 식별한다. 변경이 보호 자산과 공격면, 기존 시험 결과에 미치는 영향을 분석한다. 영향 수준에 따라 문서 갱신, Delta Evaluation, 재시험이나 신규 평가가 필요할 수 있다.

## 슬라이드 24. 개발 단계에서 보안 요구를 설계 항목으로 바꾼다
먼저 PIN, Key, Account Data와 Firmware의 흐름과 신뢰 경계를 표시한다. 각 위협에 예방, 탐지와 반응 통제를 배치한다. Requirement ID를 설계, Source와 Test Case에 연결하면 평가 직전에 증빙을 다시 만드는 낭비를 줄일 수 있다.

## 슬라이드 25. TIT-ICRFReader는 제품 역할부터 확인해야 한다
현재 저장소의 장치는 STM32와 PN5180을 사용하는 Contactless Reader Firmware로 보인다. 이 장치가 독립적으로 PIN, Key Management나 SRED를 수행하지 않는 Component라면 전체 PCI PTS POI 대상과 동일하지 않을 수 있다. 최종 판매 제품의 경계와 민감 기능을 확정한 뒤 적용 범위를 판단해야 한다.

## 슬라이드 26. 기억해야 할 핵심
첫째, 장치 유형과 신청 기능으로 평가 범위를 정한다. 둘째, 요구된 Attack Potential 이상이 되도록 물리·논리·Interface 보호를 설계한다. 셋째, 제조와 KIF, 배포, 변경과 취약점 대응까지 DTR에 맞는 증빙으로 연결한다.

## 슬라이드 27. PCI PTS를 한 문장으로 정리하면
PCI PTS는 평가된 POI 장치가 현실적인 공격과 운영 변화 속에서도 PIN과 Key, 신청한 민감 기능을 계속 보호함을 증명하는 제품 보안 체계이다. 제품과 절차, 시험과 승인 정보가 함께 유지될 때 장치 신뢰가 성립한다.
